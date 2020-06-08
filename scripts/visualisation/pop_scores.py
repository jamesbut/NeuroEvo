# This script is suppose to read in the population data
# which includes the genome fitnesses and plot the average
# population fitness and the best so far fitness

import numpy as np
import matplotlib.pyplot as plt
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
def fill_remaining_generations(exp_data):

    # Find max number of generations
    max_gens = 0
    for data in exp_data:
        if data.shape[0] > max_gens:
            max_gens = data.shape[0]

    # Fill remianing generations
    for i, data in enumerate(exp_data):
        final_gen_2d_arr = np.array([data[-1,:]])
        while data.shape[0] < max_gens:
            data = np.concatenate((data, final_gen_2d_arr))
        exp_data[i] = data

    return exp_data

def read_experiment_data(folder_names):

    exp_data = [] 

    for name in folder_names:
        exp_data.append(read_run_data(name))

    exp_data = fill_remaining_generations(exp_data)

    return np.rollaxis(np.dstack(exp_data), -1)

# Keeps track of how many files are read,
# hence how many generations there are
num_gens = 0

# Read data from all generations from csv files
def read_run_data(folder_name):

    # Path where the data is stored from the main programs runs
    full_folder_path = parent_dir + '/../data/' + folder_name

    data = []
    file_num = 1
    global num_gens

    # Continue until no more generational files can be found
    while True:

        full_file_name = full_folder_path + '/population-gen_' + str(file_num)

        gen_data = []

        try:
            with open(full_file_name) as data_file:
                csv_reader = csv.reader(data_file, delimiter=',')

                #Only take the first column which is the genome fitness
                for row in csv_reader:
                    gen_data.append(row[0])

                data.append(gen_data)

            file_num += 1
            num_gens += 1

        except FileNotFoundError:

            if file_num == 1:
                print("Could not find population file!")
                print("Terminating!..")
                sys.exit()

            break

    num_gens = 0

    return np.array(data).astype(np.float)

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

# Calculates the average best fitness so far over all the experimental runs
def calculate_avg_best_fitness_so_far(data):

    num_gens = data.shape[1]
    num_runs = data.shape[0]
    best_so_far_fitnesses = np.zeros((num_runs, num_gens))

    for i, d in enumerate(data):
        best_so_far_fitnesses[i] = calculate_best_fitness_so_far(d)

    best_so_far_means = np.mean(best_so_far_fitnesses, axis=0) 

    return best_so_far_means


# Taking all the fitnesses for all the individuals at each generation
# return a vector of the best score so far for each generation
def calculate_best_fitness_so_far(data):

    best_scores = []

    # Put best scores for each generation in best scores
    for gen in data:
        best_scores.append(max(gen))

    best_score_so_far = [best_scores[0]]

    # Find the best score so far for each gen to plot
    for i in range(1, len(best_scores)):
        if best_scores[i] > best_score_so_far[i-1]:
            best_score_so_far.append(best_scores[i])
        else:
            best_score_so_far.append(best_score_so_far[i-1])

    return np.array(best_score_so_far)

# Calculate the median average fitness over all experimental runs
def calculate_median_avg_fitness(data):

    avg_fitnesses = calculate_avg_fitnesses(data)
    median_avg_fitnesses = np.median(avg_fitnesses, axis=0) 
    return median_avg_fitnesses

# Calculate the 75th quantile of average fitness over all experimental runs
def calculate_high_quantile_avg_fitness(data):

    avg_fitnesses = calculate_avg_fitnesses(data)
    high_quantile_avg_fitnesses = np.quantile(avg_fitnesses, 0.75, axis=0) 
    return high_quantile_avg_fitnesses

# Calculate the 25th quantile of average fitness over all experimental runs
def calculate_low_quantile_avg_fitness(data):

    avg_fitnesses = calculate_avg_fitnesses(data)
    low_quantile_avg_fitnesses = np.quantile(avg_fitnesses, 0.25, axis=0) 
    return low_quantile_avg_fitnesses

def calculate_avg_fitnesses(data):

    num_gens = data.shape[1]
    num_runs = data.shape[0]
    average_fitnesses = np.zeros((num_runs, num_gens))

    for i, d in enumerate(data):
        average_fitnesses[i] = calculate_average_fitness(d)

    return average_fitnesses

# Returns a vector of the average fitness of the population
# at each generation
def calculate_average_fitness(data):

    return np.mean(data, axis=1)

# Draws best so far and average population fitness on
# same plot
def draw_plot(best_so_far_fitnesses, average_fitnesses):

    # Create x vector of generations
    x = np.arange(1, num_gens+1)

    plt.plot(x, best_so_far_fitnesses)
    plt.plot(x, average_fitnesses)
    plt.show()

def plot_experiment(best_so_far, median_avg, high_q, low_q, colour):

    # Create x vector of generations
    x = np.arange(1, data.shape[1]+1)

    plotted_data_best = np.column_stack((x, best_so_far))
    plotted_data_avg = np.column_stack((x, median_avg))
    plotted_data_high_quantile = np.column_stack((x, high_q))
    plotted_data_low_quantile = np.column_stack((x, low_q))
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

    plt.fill_between(x, low_quantile_avg_fitnesses, high_quantile_avg_fitnesses, 
                     color=colour, alpha=0.1)


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

    exp_plot_colours = ['b', 'r', 'g']

    # Import data
    for i, exp_folder in enumerate(data_folder_names):

        data = read_experiment_data(exp_folder)

        # Calculate required measures
        avg_best_so_far_fitnesses = calculate_avg_best_fitness_so_far(data)
        median_avg_fitnesses = calculate_median_avg_fitness(data)
        high_quantile_avg_fitnesses = calculate_high_quantile_avg_fitness(data)
        low_quantile_avg_fitnesses = calculate_low_quantile_avg_fitness(data)

        # Debug printing
        #print(data)
        #print(avg_best_so_far_fitnesses)
        #print(avg_avg_fitnesses)

        # Draw plots
        plot_experiment(avg_best_so_far_fitnesses, median_avg_fitnesses, 
                        high_quantile_avg_fitnesses, low_quantile_avg_fitnesses,
                        exp_plot_colours[i])
    plt.show()
