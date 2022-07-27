import os
import sys
import numpy as np
import json


# Read useful data about winning agents from experiment
def read_agent_data(exp_data_path, winner_file_name):

    # Get directories in data_dir_path
    run_folder_paths = get_sub_folders(exp_data_path)

    # If there are no directories in exp_data_path, then look for data in 
    # exp_data_path
    if not run_folder_paths:
        run_folder_paths = [exp_data_path]

    # Append winner file name to folder paths
    for i in range(len(run_folder_paths)):
        run_folder_paths[i] += '/' + winner_file_name + '.json'

    # Read data
    fitnesses, genos, phenos, domain_params = read_data(run_folder_paths)

    return fitnesses, genos, phenos, domain_params, run_folder_paths


# Read data from evolutionary runs
def read_evo_data(exp_data_path):

    # Get sub folders in data_dir (different run folders)
    run_folder_paths = get_sub_folders(exp_data_path)
    if not run_folder_paths:
        run_folder_paths = [exp_data_path]

    exp_mean_fitnesses = []
    exp_best_fitnesses = []
    for rfp in run_folder_paths:
        rfp_means = rfp + '/mean_fitnesses'
        rfp_bests = rfp + '/best_fitnesses'

        # Read data and convert to floats
        mean_fitnesses = list(map(float, read_list(rfp_means)))
        best_fitnesses = list(map(float, read_list(rfp_bests)))

        exp_mean_fitnesses.append(mean_fitnesses)
        exp_best_fitnesses.append(best_fitnesses)

    return np.array(exp_mean_fitnesses), np.array(exp_best_fitnesses), run_folder_paths


# Read new data format that uses json files
def read_data(folder_paths):

    fitnesses = []
    genotypes = []
    phenotypes = []
    domain_params = []

    for fp in folder_paths:
        try:
            with open(fp) as agent_file:
                agent = json.load(agent_file)

                fitnesses.append(agent['Organism']['fitness'])
                genotypes.append(agent['Organism']['genes'])
                phenotypes.append(agent['Organism']['Phenotype']['traits'])
                # domain_params.append(agent['env']['domain_params'])

        except FileNotFoundError:
            sys.exit("Could not find file named: " + fp)

    return np.array(fitnesses), np.array(genotypes), np.array(phenotypes), \
           np.array(domain_params)


def get_sub_folders(dir_path, recursive=True, append_dir_path=True,
                    num_top_dirs_appended=0, final_sub_dirs_only=False,
                    num_top_dirs_removed=0, sort_by_suffix_num=False):

    # If appended top directories is given, auto turn off append full directory path
    if num_top_dirs_appended:
        append_dir_path = False

    if os.path.isdir(dir_path):
        # Walk directory
        walk = list(os.walk(dir_path))

        # If final subdirectories are needed only
        if final_sub_dirs_only:
            folder_names = [x[0] for x in walk if not x[1]]
        elif recursive and append_dir_path:
            folder_names = [x[0] for x in walk][1:]
        elif not recursive:
            folder_names = walk[0][1]
            if append_dir_path:
                folder_names = list(map(lambda fn: dir_path + '/' + fn, folder_names))

        # Sort results
        folder_names = sorted(folder_names)
        if sort_by_suffix_num:
            folder_names = _sort_by_suffix_num(folder_names)

        # Remove top directories
        folder_names = [remove_dirs_from_path(x, num_top_dirs_removed)
                        for x in folder_names]

        # Append top directories
        if num_top_dirs_appended > 0:
            top_dirs = '/'.join(dir_path.split('/')[-num_top_dirs_appended:])
            folder_names = [top_dirs + '/' + fn for fn in folder_names]

    else:
        raise NotADirectoryError("{} is not a directory".format(dir_path))

    return folder_names


# Sort directories by the number at the end of the file
def _sort_by_suffix_num(directories):

    def parse_suffix(directory):
        return int(directory.split('_')[-1])

    directories.sort(key=parse_suffix)
    return directories


# Read list from file
def read_list(file_path):

    with open(file_path, 'r') as f:
        lst = f.readlines()

    # Remove carriage returns
    lst = [val.strip() for val in lst]

    return lst


# Removes a number of directories from a path string
def remove_dirs_from_path(path, num_dirs_removed=1):
    return '/'.join(path.split('/')[num_dirs_removed:])


# Read json
def read_json(file_path):
    with open(file_path, 'r') as f:
        return json.load(f)
