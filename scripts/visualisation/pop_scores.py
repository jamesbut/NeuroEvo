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

# Append scripts/ directory to sys.path
current_path = os.path.abspath(getsourcefile(lambda:0))
current_dir = os.path.dirname(current_path)
parent_dir = current_dir[:current_dir.rfind(os.path.sep)]
sys.path.insert(0, parent_dir)

# Keeps track of how many files are read,
# hence how many generations there are
num_gens = 0

# Read data from all generations from csv files
def read_csv_files(folder_name):

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
    print(np.array(data))

    return np.array(data).astype(np.float)

# Finds most recently created folder
def get_most_recent_folder():

    folder = max(glob.glob(os.path.join(parent_dir + '/../data/', '*/')), key=os.path.getmtime)
    split_folder_str = folder.split("/")
    return split_folder_str[-2]

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

    return best_score_so_far

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

    # Take folder name as command line argument.
    # NOTE: This is not the full folder path, that is accounted
    # for within the read_csv_files function
    # If no folder is given the most recently generated
    # folder is used

    if len(sys.argv) < 2:
        data_folder_name = get_most_recent_folder();
    else:
        data_folder_name = sys.argv[1]

    # Import data
    data = read_csv_files(data_folder_name)

    # Calculate required measures
    best_so_far_fitnesses = calculate_best_fitness_so_far(data)
    average_fitnesses = calculate_average_fitness(data)

    # Debug printing
    #print(data)
    print(best_so_far_fitnesses)
    #print(average_fitnesses)

    # Draw plots
    draw_plot(best_so_far_fitnesses, average_fitnesses)

