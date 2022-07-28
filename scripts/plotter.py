import sys
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
from typing import Optional, List
from data import read_agent_data, read_evo_data, get_sub_folders
from command_line import parse_axis_limits, retrieve_flag_args

np.set_printoptions(suppress=True)


# Take data with a discrete third dimension and plot each discrete group separately
def _plot_discrete_scatter(train_phenotypes, colour_vals):

    train_phenotypes = np.concatenate((train_phenotypes, colour_vals[:, None]),
                                      axis=1)
    # Order by colour values (domain parameters)
    train_phenotypes = train_phenotypes[train_phenotypes[:, 2].argsort()]

    # Get unique discrete colour values
    def get_unique_colour_vals():
        unique_colour_vals = []
        for colour in train_phenotypes[:, 2]:
            if colour not in unique_colour_vals:
                unique_colour_vals.append(colour)
        return unique_colour_vals

    unique_colour_vals = get_unique_colour_vals()

    # Colours for each domain parameter
    param_colour = ['thistle', 'gold', 'darkorange']

    # Scatter plot for each different colour value
    for i, colour in enumerate(unique_colour_vals):
        plot_phenos = []
        for pheno in train_phenotypes:
            if pheno[2] == colour:
                plot_phenos.append(pheno)
        plot_phenos = np.array(plot_phenos)
        plt.scatter(plot_phenos[:, 0], plot_phenos[:, 1],
                    label='Engine power: ' + str(colour) + ' train data',
                    c=param_colour[i])


def _plot_phenos_scatter(train_phenotypes=None, colour_vals=None,
                         discrete_colours: bool = False, test_phenotypes=None,
                         plot_axis_lb=None, plot_axis_ub=None,
                         plot_evo_starts: Optional[List[str]] = None):

    # Plot training data
    if train_phenotypes is not None:

        # If there are 3d values (which will typically be either fitnesses or
        # domain params)
        if colour_vals is not None:

            # If the third dimentional values are discrete
            if discrete_colours:
                _plot_discrete_scatter(train_phenotypes, colour_vals)

            # If the third dimensional values are continous
            else:
                plt.scatter(train_phenotypes[:, 0], train_phenotypes[:, 1],
                            c=colour_vals, cmap='plasma', alpha=1.)
                plt.colorbar()

        else:
            plt.scatter(train_phenotypes[:, 0], train_phenotypes[:, 1])

    # Plot test data
    if test_phenotypes is not None:
        plt.scatter(test_phenotypes[:, 0], test_phenotypes[:, 1], alpha=.5, c='black',
                    label='IE manifold')

    '''
    # Plot evo starts
    if plot_evo_starts is not None:

        # Read in UC start centroid by giving file name in command line
        UC_start_file_name = plot_evo_starts[0]
        # TODO: Use data_dir_path and winner_file_name from config
        uc_file_path = 'data/' + UC_start_file_name + '/best_winner_so_far.json'
        UC_start_centroid = Agent(agent_path=uc_file_path).genotype

        # Read in IE winner from command line
        IE_winner_file_name = plot_evo_starts[1]
        IE_file_path = 'data/' + IE_winner_file_name + '/best_winner_so_far.json'
        IE_winner = Agent(agent_path=IE_file_path).weights

        # Plot DE start
        plt.scatter(0., 0., marker='x', c='red', s=[10.], label='DE start centroid')
        # Plot UC start
        plt.scatter(UC_start_centroid[0], UC_start_centroid[1], marker='x',
                    c='green', s=[10.], label='Universal Controller start centroid')
        # Plot IE winner
        plt.scatter(IE_winner[0], IE_winner[1], marker='D',
                    c='blue', s=[10.], label='IE winner')

        # Plot initial sigmas
        de_sigma = plt.Circle((0., 0.), 1.0, fill=False, linestyle='--',
                                edgecolor='red')
        plt.gcf().gca().add_artist(de_sigma)
        uc_sigma = plt.Circle((UC_start_centroid[0], UC_start_centroid[1]),
                                1.0, fill=False, linestyle='--', edgecolor='green')
        plt.gcf().gca().add_artist(uc_sigma)
    '''

    # Set axis limit if given
    if plot_axis_lb is not None and plot_axis_ub is not None:
        print('plot_axis_lb:', plot_axis_lb)
        print('plot_axis_ub:', plot_axis_ub)
        plt.xlim([plot_axis_lb[0], plot_axis_ub[0]])
        plt.ylim([plot_axis_lb[1], plot_axis_ub[1]])

    plt.legend()

    plt.xlabel('Weight 1')
    plt.ylabel('Weight 2')

    # Save figure
    print('Saving figure..')
    plt.gcf().set_size_inches(10, 10)
    plt.savefig('figures/pheno_figure.png', bbox_inches='tight',
                pad_inches=0.05, dpi=500, figsize=(80, 60))

    plt.show()


def _plot_exp_evo_data(mean_bests, median_bests, lq_bests, uq_bests, best_bests,
                       mean_means, median_means, lq_means, uq_means, colour,
                       plot_mean_bests: bool = False, plot_median_bests: bool = True,
                       plot_q_bests: bool = False, plot_best_bests: bool = True,
                       plot_mean_means: bool = True, plot_median_means: bool = False,
                       plot_q_means: bool = False, x_axis_max: Optional[int] = None):

    # Inner class to build plot data
    class PlotData:

        def __init__(self, x_axis: list, x_axis_max: Optional[int]):

            self.data = np.empty([0, len(x_axis), 2])
            self.line_styles: List[str] = []
            self.line_widths: List[float] = []
            self.gens = x_axis
            self.x_axis_max = x_axis_max
            # I need a way to record the particular data that has been plotted
            # in order to label the legend later
            self.labels: List[Optional[str]] = []

        def add_data(self, data: np.ndarray, line_style: str, line_width: float,
                     label: Optional[str]):

            # Prepare data for plotting
            data_stacked = np.column_stack((self.gens, data))

            # Concatenate with old data
            self.data = np.concatenate((self.data, np.array([data_stacked])))

            # Concatenate new line styles and widths with old
            self.line_styles.append(line_style)
            self.line_widths.append(line_width)

            self.labels.append(label)

        def plot(self):

            # Plot data
            for i in range(self.data.shape[0]):
                plt.plot(self.data[i, :self.x_axis_max, 0],
                         self.data[i, :self.x_axis_max, 1],
                         color=colour, linestyle=self.line_styles[i],
                         linewidth=self.line_widths[i])

    # Create x axis of generations
    gens = np.arange(1, median_bests.shape[0] + 1)
    #x_axis_ticks = range(gens[0], gens[-1])
    #plt.xticks(x_axis_ticks)

    data = PlotData(gens, x_axis_max)

    # Plot best winner so far data
    if plot_mean_bests:
        data.add_data(mean_bests, '-', 1., 'best winner so far means')
    if plot_median_bests:
        data.add_data(median_bests, '-', 1., 'best winner so far medians')
    if plot_q_bests:
        data.add_data(lq_bests, '--', 0.25, None)
        data.add_data(uq_bests, '--', 0.25, None)
    if plot_best_bests:
        data.add_data(best_bests, ':', 1., 'best winner so far best run')

    # Plot population average data
    if plot_mean_means:
        data.add_data(mean_means, '--', 1., 'mean average population fitness')
    if plot_median_means:
        data.add_data(median_means, '--', 1., 'median average population fitness')
    if plot_q_means:
        data.add_data(lq_means, '--', 0.25, None)
        data.add_data(uq_means, '--', 0.25, None)

    # Plot data
    data.plot()

    # Fill between IQRs
    if plot_q_means:
        plt.fill_between(gens, lq_means, uq_means, color=colour, alpha=0.1)
    if plot_q_bests:
        plt.fill_between(gens, lq_bests, uq_bests, color=colour, alpha=0.1)

    # Return labels of plotted data
    return data.labels


def _fitness_analysis(fitnesses, folder_paths, verbosity: bool) -> float:

    max_arg = np.argmax(fitnesses)
    max_fitness = fitnesses[max_arg]
    max_file = folder_paths[max_arg]

    if verbosity:
        print("Max fitness: {}              File: {}".format(max_fitness, max_file))

    mean_fitness = np.mean(fitnesses)
    if verbosity:
        print("Mean fitness:", mean_fitness)

    median_fitness = np.median(fitnesses)
    if verbosity:
        print("Median fitness:", median_fitness)

    min_arg = np.argmin(fitnesses)
    min_fitness = fitnesses[min_arg]
    min_file = folder_paths[min_arg]

    if verbosity:
        print("Min fitness: {}              File: {}".format(min_fitness, min_file))

    return max_fitness, max_file


# Calculate best fitnesses so far from the best fitnesses for each generation
def calculate_best_fitnesses_so_far(best_fitnesses):

    def create_best_fitnesses_so_far(run_best_fitnesses):
        best_fitnesses_so_far = np.empty_like(run_best_fitnesses)
        best_fitness_so_far = run_best_fitnesses[0]
        # Calculate best winner so far at each generation
        for i, f in enumerate(run_best_fitnesses):
            if f > best_fitness_so_far:
                best_fitness_so_far = f
            best_fitnesses_so_far[i] = best_fitness_so_far
        return best_fitnesses_so_far

    # For each evolutionary run
    return np.apply_along_axis(create_best_fitnesses_so_far, 1, best_fitnesses)


# Read pheno data of experiment
def _read_exp(exp_data_path, winner_file_name, verbosity, colour_params):

    # Read agent data
    if exp_data_path is not None:
        fitnesses, genos, phenos, params, folder_paths = \
            read_agent_data(exp_data_path, winner_file_name)

    if verbosity:
        print("Fitnesses:\n", fitnesses)
        print("Genotypes:\n", genos)
        print("Phenotypes:\n", phenos)
        print("Params:\n", params[0])

    # Flatten params for now - this might not work when training has been on
    # more than one param
    params = params.flatten()

    # Provide fitness analysis
    max_fitness, max_file = _fitness_analysis(fitnesses, folder_paths, verbosity)

    # Determine colour values for plotting
    colour_vals = fitnesses
    if colour_params:
        colour_vals = params

    return max_fitness, max_file, phenos, colour_vals


def read_and_plot_phenos(exp_data_path=None, winner_file_name=None, test_data=None,
                         group=False, colour_params=False, print_numpy_arrays=False,
                         verbosity=True, plot_axis_lb=None, plot_axis_ub=None,
                         plot_evo_starts: Optional[List[str]] = None):

    print('exp_data_path:', exp_data_path)

    # Get all experiments from group
    if group:
        exp_data_paths = get_sub_folders(exp_data_path, recursive=False,
                                         sort_by_suffix_num=True)
    # Otherwise just process given experiment
    else:
        exp_data_paths = [exp_data_path]
        verbosity = True

    # Print full numpy arrays
    if print_numpy_arrays:
        np.set_printoptions(threshold=sys.maxsize)

    max_exp_fitnesses = []

    # For all experiments, collect pheno data
    for i, exp_data_path in enumerate(exp_data_paths):
        print(exp_data_path)

        # Read pheno data
        max_fitness, max_file, phenos, colour_vals = \
            _read_exp(exp_data_path, winner_file_name, verbosity, colour_params)

        # Plot pheno data
        if verbosity:
            _plot_phenos_scatter(phenos, colour_vals, colour_params, test_data,
                                 plot_axis_lb, plot_axis_ub, plot_evo_starts)

        # Keep track of max fitness
        max_exp_fitnesses.append(max_fitness)

    if group:
        # Find the experiment with the maximum fitness of the group and run again
        print('**********************************************')
        print('*   Experiment with max fitness in group     *')
        print('**********************************************')

        # Calculate exp with max fitness in group
        group_max_exp_fitness_arg = np.argmax(max_exp_fitnesses)

        # Read experiment data
        _, phenos, colour_vals = _read_exp(
            exp_data_paths[group_max_exp_fitness_arg], winner_file_name,
            True, colour_params
        )

        # Scatter plot
        _plot_phenos_scatter(phenos, colour_vals, colour_params, test_data,
                             plot_axis_lb, plot_axis_ub, plot_evo_starts)

        print('**********************************************')


# Determines which experiment to plot when a sub group of experiments is given

# gen_one_max is a boolean which determines which experiment in the sub group
# to use.
# If gen_one_max is false, the experiment with the best winner so far with the
# highest fitness is chosen, otherwise the experiment with the highest best
# winner so far fitness at generation 1 is chosen
def _determine_experiment_to_plot(exp_data_path: str, winner_file_name: str,
                                  gen_one_max: bool, verbosity: bool) -> str:

    # Check whether exp_data_path is an experiment directory or subgroup
    # directory
    if 'exp' in exp_data_path.split('/')[-1]:
        return exp_data_path

    # A subgroup directory
    else:

        # Get subgroup experiment paths
        group_exp_data_paths = get_sub_folders(exp_data_path,
                                               recursive=False,
                                               sort_by_suffix_num=True)

        max_fitnesses: List[float] = []
        max_fitness_files: List[str] = []
        for edp in group_exp_data_paths:

            # Calculate run with greatest best fitness so far at generation 1
            if gen_one_max:

                _, best_fitnesses, run_folder_paths = read_evo_data(edp)
                max_arg = np.argmax(best_fitnesses[:, 0])
                max_fitnesses.append(best_fitnesses[max_arg, 0])
                max_fitness_files.append(run_folder_paths[max_arg])

            # Calculate max fitness of best winners so far for the experiments
            # in the group
            else:

                max_fitness, max_file, _, _ = _read_exp(edp, winner_file_name,
                                                        verbosity, False)
                max_fitnesses.append(max_fitness)
                max_fitness_files.append(max_file)

        if verbosity:
            print('Max fitnesses:', max_fitnesses)

        max_fitness_arg = np.argmax(max_fitnesses)

        # Print run file with the max fitness
        print('Max fitness run:',
              max_fitness_files[max_fitness_arg]
                .removesuffix('best_winner_so_far.json'))

        # Choose experiment with the max fitness
        return group_exp_data_paths[max_fitness_arg]


# Prompt for legend lables
def _prompt_legend_labels(exp_data_dirs) -> list[str]:

    legend_labels: list[str] = []

    for exp_data_dir in exp_data_dirs:
        print(exp_data_dir)
        legend_labels.append(input('Legend label? '))

    return legend_labels


# Plot legend
def _plot_legend(prompt_legend_labels: bool, exp_data_dirs: List[str],
                 plot_colours: List[str], plotted_labels: List[Optional[str]]):

    # If legend labels are to be prompted
    if prompt_legend_labels:

        # Prompt experiment labels
        exp_labels = _prompt_legend_labels(exp_data_dirs)

        # Suffix with the plotted labels
        legend_labels: List[str] = []
        for i, el in enumerate(exp_labels):
            for pl in plotted_labels[i]:
                if pl is not None:
                    legend_labels.append(el + ' ' + pl)

        # Plot legend
        plt.gca().legend(tuple(legend_labels))

    else:

        # Create legend that labels the colours as the experiment directory names
        handles = [mpatches.Patch(color=exp_colour, label=exp_label)
                   for exp_label, exp_colour in zip(exp_data_dirs, plot_colours)]

        # Plot legend
        plt.legend(handles=handles)


def read_and_plot_evo_data(exp_data_dirs, data_dir_path, winner_file_name,
                           gen_one_max: bool = False,
                           plot_mean_bests: bool = False,
                           plot_median_bests: bool = True,
                           plot_q_bests: bool = False,
                           plot_best_bests: bool = True,
                           plot_mean_means: bool = True,
                           plot_median_means: bool = False,
                           plot_q_means: bool = False,
                           verbosity: bool = False,
                           prompt_legend_labels: bool = False,
                           x_axis_max: Optional[int] = None):

    # Prefix exp data directory with data path
    exp_data_paths = [data_dir_path + edd for edd in exp_data_dirs]
    exp_plot_colours = ['b', 'r', 'g', 'm', 'y', 'c']
    # Labels of plotted data
    plotted_labels: List[Optional[str]] = []

    for i, exp_data_path in enumerate(exp_data_paths):

        # Determine which experiment to plot
        exp_data_path = _determine_experiment_to_plot(
            exp_data_path, winner_file_name,
            gen_one_max, verbosity
        )
        print('Experiment data path:', exp_data_path)

        # Read experiment data
        mean_fitnesses, best_fitnesses, _ = read_evo_data(exp_data_path)
        if verbosity:
            print('Best fitnesses of run:\n', best_fitnesses)

        # Calculate best fitnesses so far
        best_fitnesses_so_far = calculate_best_fitnesses_so_far(best_fitnesses)

        # Print number of runs
        print('Number of runs:', len(best_fitnesses))

        # Calculate statistics
        mean_best_fitnesses = np.mean(best_fitnesses_so_far, axis=0)
        median_best_fitnesses = np.median(best_fitnesses_so_far, axis=0)
        lq_best_fitnesses = np.quantile(best_fitnesses_so_far, 0.25, axis=0)
        uq_best_fitnesses = np.quantile(best_fitnesses_so_far, 0.75, axis=0)
        best_best_fitnesses = np.max(best_fitnesses_so_far, axis=0)

        mean_mean_fitnesses = np.mean(mean_fitnesses, axis=0)
        median_mean_fitnesses = np.median(mean_fitnesses, axis=0)
        lq_mean_fitnesses = np.quantile(mean_fitnesses, 0.25, axis=0)
        uq_mean_fitnesses = np.quantile(mean_fitnesses, 0.75, axis=0)

        # Plot experiment data and return labels of plotted data
        labels = _plot_exp_evo_data(
            mean_best_fitnesses, median_best_fitnesses,
            lq_best_fitnesses, uq_best_fitnesses, best_best_fitnesses,
            mean_mean_fitnesses, median_mean_fitnesses,
            lq_mean_fitnesses, uq_mean_fitnesses,
            exp_plot_colours[i], plot_mean_bests, plot_median_bests,
            plot_q_bests, plot_best_bests, plot_mean_means,
            plot_median_means, plot_q_means, x_axis_max)

        plotted_labels.append(labels)
        print('-' * 50)

    # Plot legend
    _plot_legend(prompt_legend_labels, exp_data_dirs,
                 exp_plot_colours, plotted_labels)

    plt.xlabel('Generation')
    plt.ylabel('Fitness')

    # Save figure
    # plt.savefig('figures/evo_figure.png', bbox_inches='tight',
    #             pad_inches=0.05, dpi=500)

    plt.show()


if __name__ == '__main__':

    # Can pass in entire experiment group
    if '--group' in sys.argv:
        exp_dir = sys.argv[3]
    # Or just single experiment
    else:
        exp_dir = sys.argv[2]

    # Plot phenotype data
    if '--pheno' in sys.argv:

        # Get axis limits from command line
        plot_axis_lb, plot_axis_ub = parse_axis_limits(sys.argv)

        read_and_plot_phenos(
            '../data/' + exp_dir,
            'best_winner_so_far',
            group=True if '--group' in sys.argv else False,
            colour_params=True if '--colour-params' in sys.argv else False,
            print_numpy_arrays=True if '--print-numpy-arrays' in sys.argv
                                    else False,
            verbosity=True if '--verbosity' in sys.argv else False,
            plot_axis_lb=plot_axis_lb,
            plot_axis_ub=plot_axis_ub
        )

    # Plot evolutionary run data
    elif '--evo' in sys.argv:

        ######################
        # Parse what to plot #
        ######################

        # Plot either mean or median of best winners so far
        plot_mean_bests = True if '--mean-bests-on' in sys.argv else False
        plot_median_bests = not plot_mean_bests
        # Plot IQR of best winners so far
        plot_q_bests = True if '--q-bests-on' in sys.argv else False
        # Plot best run of best winners so far
        plot_b_bests = False if '--best-bests-off' in sys.argv else True

        # Plot either mean or median of average population fitness
        plot_mean_means = False if '--mean-means-off' in sys.argv else True
        plot_median_means = True if '--median-means-on' in sys.argv else False
        # Plot IQR of average population fitness
        plot_q_means = True if '--q-means-on' in sys.argv else False

        # Plot run with maximum best so far fitness as generation 1
        gen_one_max = True if '--gen-one-max' in sys.argv else False

        verbosity = True if '--verbosity' in sys.argv else False
        # Print full numpy arrays
        if verbosity:
            np.set_printoptions(threshold=sys.maxsize)

        # Prompot for legend labels
        prompt_legend_labels = True if '--legend-labels' in sys.argv else False

        # x-axis max
        x_axis_max: Optional[list[str]] = \
            retrieve_flag_args('--x-axis-max', sys.argv)
        # Convert to int
        if x_axis_max is not None:
            x_axis_max = int(x_axis_max[0])

        # Split comma separated experiment directories
        exp_data_dirs = exp_dir.split(',')

        read_and_plot_evo_data(
            exp_data_dirs, '../data/',
            'best_winner_so_far',
            gen_one_max, plot_mean_bests, plot_median_bests,
            plot_q_bests, plot_b_bests,
            plot_mean_means, plot_median_means, plot_q_means, verbosity,
            prompt_legend_labels, x_axis_max
        )
