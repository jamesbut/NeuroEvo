# This script is suppose to read in the population data
# which includes the genome fitnesses and plot the average
# population fitness and the best so far fitness

import numpy as np
np.set_printoptions(suppress=True)
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches 
import csv
import sys
import os
import glob
from inspect import getsourcefile
from plotter import *

# Append scripts/ directory to sys.path
current_path = os.path.abspath(getsourcefile(lambda:0))
current_dir = os.path.dirname(current_path)
parent_dir = current_dir[:current_dir.rfind(os.path.sep)]
sys.path.insert(0, parent_dir)

# Fills remaining generations with the value from the last generation
#def fill_remaining_generations(exp_data):
#
#    # Find max number of generations
#    max_gens = 0
#    for data in exp_data:
#        if data.shape[0] > max_gens:
#            max_gens = data.shape[0]
#
#    # Fill remianing generations
#    for i, data in enumerate(exp_data):
#        final_gen_2d_arr = np.array([data[-1,:]])
#        while data.shape[0] < max_gens:
#            data = np.concatenate((data, final_gen_2d_arr))
#        exp_data[i] = data
#
#    return exp_data

def process_string_list(l):

    #Remove carriage returns and convert to floats
    for i, value in enumerate(l):
        l[i] = float(value.strip())


def read_experiment_data(folder_names):

    mean_fitnesses = []
    best_fitnesses = []

    for name in folder_names:
        mf, bf = read_run_data(name)
        mean_fitnesses.append(mf)
        best_fitnesses.append(bf)

    print(best_fitnesses)

    return np.array(mean_fitnesses), np.array(best_fitnesses)

#Read in fitness means and best fitnesses so far for the run
def read_run_data(folder_name):

    full_folder_path = parent_dir + '/../data/' + folder_name
    means_file_name = full_folder_path + '/mean_fitnesses'
    best_fitnesses_file_name = full_folder_path + '/best_fitnesses'

    mean_fitnesses = []

    try: 
        with open(means_file_name, 'r') as means_file:
           mean_fitnesses = means_file.readlines()
    except FileNotFoundError:
        print("Could not find file: ", means_file_name)
        sys.exit()

    best_fitnesses = []

    try:
        with open(best_fitnesses_file_name, 'r') as best_file:
            best_fitnesses = best_file.readlines()
    except FileNotFoundError:
        print("Could not find file: ", best_fitnesses_file_name)
        sys.exit()

    #Clean up data
    process_string_list(mean_fitnesses)
    process_string_list(best_fitnesses)

    return mean_fitnesses, best_fitnesses

# Finds most recent run in experiment 1
def get_default_exp_and_run():

    folder = max(glob.glob(os.path.join(parent_dir + '/../data/exp_1/', '*/')), 
                 key=os.path.getmtime)
    split_folder_str = folder.split("/")
    return [["exp_1/" + split_folder_str[-2]]]

def get_run_folder_names(exp_num):

    folders = glob.glob(os.path.join(parent_dir + '/../data/exp_' + exp_num + '/', '*/'))
    split_folders = []
    for folder in folders:
        split_folder = folder.split("/")
        folder_name = split_folder[-3] + "/" + split_folder[-2] 
        split_folders.append(folder_name)

    return split_folders

def plot_experiment(mean_best_so_far, median_means, uq_means, lq_means, colour):

    # Create x vector of generations
    x = np.arange(1, mean_best_so_far.shape[0]+1)

    plotted_data_best = np.column_stack((x, mean_best_so_far))
    plotted_data_avg = np.column_stack((x, median_means))
    plotted_data_high_quantile = np.column_stack((x, uq_means))
    plotted_data_low_quantile = np.column_stack((x, lq_means))
    plotted_data = np.array([plotted_data_best, plotted_data_avg, 
                             plotted_data_high_quantile, plotted_data_low_quantile])

    xlabel = 'Generation'
    ylabel = 'Fitness'

    legend_labels = ["Best fitness so far", "Population avg fitness", 
                     "High quantile", "Low quantile"]
    line_styles = ['--', '-', '--', '--']
    line_widths = [1., 1., 0.25, 0.25]

    for i in range(plotted_data.shape[0]):
        plt.plot(plotted_data[i,:,0], plotted_data[i,:,1], 
                 color=colour, linestyle=line_styles[i],
                 linewidth=line_widths[i])

    plt.fill_between(x, lq_means, uq_means, color=colour, alpha=0.1)

if __name__ == '__main__':

    # Take *exp number* *run folder name* as arguments
    # They are both optional
    # One can also give a comma seperated list of exp numbers to plot
    # on the same plot

    # If no arguments are given the most recently generated
    # population in experiment 1 is used.
    if len(sys.argv) < 2:
        data_folder_names = get_default_exp_and_run();
    # If just experiment number is given, all the runs are collected
    elif len(sys.argv) == 2:
        exp_nums = sys.argv[1].split(",")
        data_folder_names = []
        for exp_num in exp_nums:
            data_folder_names.append(get_run_folder_names(exp_num))
    elif len(sys.argv) == 3:
        exp_num = sys.argv[1]
        run_folder_name = sys.argv[2]
        data_folder_names = [["exp_" + exp_num + "/" + run_folder_name]]

    if not data_folder_names:
        print("Could not find folder to read from!")
        sys.exit()

    exp_plot_colours = ['b', 'r', 'g', 'm', 'y']
    legend_labels = ['Direct map', 'Baseline map', 'AE', 'VAE', 'GAN']
    legend_items = []

    # Import data
    for i, exp_folder in enumerate(data_folder_names):

        print("Exp num: " + exp_nums[i] + " Colour: " + exp_plot_colours[i])

        #Read data
        mean_fitnesses, best_fitnesses = read_experiment_data(exp_folder)

        #Calculate statistics
        mean_best_so_far_fitnesses = np.mean(best_fitnesses, axis=0)
        median_mean_fitnesses = np.median(mean_fitnesses, axis=0)  
        lq_mean_fitnesses = np.quantile(mean_fitnesses, 0.25, axis=0)
        uq_mean_fitnesses = np.quantile(mean_fitnesses, 0.75, axis=0)

        print(median_mean_fitnesses)

        plot_experiment(mean_best_so_far_fitnesses, median_mean_fitnesses,
                        uq_mean_fitnesses, lq_mean_fitnesses, exp_plot_colours[i])

        legend_items.append(mpatches.Patch(color=exp_plot_colours[i], label=legend_labels[i]))

    plt.legend(handles=legend_items)

    plt.show()
