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

    folder = max(glob.glob(os.path.join(parent_dir + '/../data/exp_1/', '*/')), key=os.path.getmtime)
    split_folder_str = folder.split("/")
    return ["exp_1/" + split_folder_str[-2]]

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

    total_best_fitness = 0.

    for d in data:
        total_best_fitness += calculate_best_fitness_so_far(d)

    return total_best_fitness / float(len(data))


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

# Calculates the average average fitness over all of the experimental runs
def calculate_avg_avg_fitness(data):

    total_avg_fitness = 0.

    for d in data:
        total_avg_fitness += calculate_average_fitness(d)

    return total_avg_fitness / float(len(data))

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

if __name__ == '__main__':

    # Take *exp number* *run folder name* as arguments
    # They are both optional

    # If no arguments are given the most recently generated
    # population in experiment 1 is used.
    if len(sys.argv) < 2:
        data_folder_names = get_default_exp_and_run();
    # If just experiment number is given, all the runs are collected
    elif len(sys.argv) == 2:
        exp_num = sys.argv[1]
        data_folder_names = get_run_folder_names(exp_num)
    elif len(sys.argv) == 3:
        exp_num = sys.argv[1]
        run_folder_name = sys.argv[2]
        data_folder_names = ["exp_" + exp_num + "/" + run_folder_name]

    # Import data
    data = read_experiment_data(data_folder_names)

    # Calculate required measures
    avg_best_so_far_fitnesses = calculate_avg_best_fitness_so_far(data)
    avg_avg_fitnesses = calculate_avg_avg_fitness(data)

    # Debug printing
    #print(data)
    #print(avg_best_so_far_fitnesses)
    #print(avg_avg_fitnesses)

    # Draw plots

    # Create x vector of generations
    x = np.arange(1, data.shape[1]+1)

    plotted_data_best = np.column_stack((x, avg_best_so_far_fitnesses))
    plotted_data_avg = np.column_stack((x, avg_avg_fitnesses))
    plotted_data = np.array([plotted_data_best, plotted_data_avg])

    xlabel = 'Generation'
    ylabel = 'Fitness'

    legend_labels = ["Best fitness so far", "Population avg fitness"]
    plot_colours = ['r', 'b']

    plot_2d_line(plotted_data, xlabel = xlabel, ylabel = ylabel, 
                 legend_labels = legend_labels, plot_colours = plot_colours)

