#ifndef _DATA_COLLECTION_H_
#define _DATA_COLLECTION_H_

/*
    This class collects data about the evolutionary
    run and prints it
*/

#include <population.h>
#include <string>
#include <optional>

namespace NeuroEvo {

template <typename G>
class DataCollector {

public:

    DataCollector(const bool uuid_folders = true);

    void collect_generational_data(Population<G>& population);

    void print_population_winner(Population<G>& population);
    void print_generational_winner(Population<G>& population);
    void print_best_winner_so_far(Population<G>& population);

    void delete_folder();

    const std::string get_folder_path() { return _folder_path; };

private:

    void print_population_to_file(Population<G>& population);

    //Path to folder in which to store
    //all data for a run
    std::string _folder_path;

    std::optional<double> best_fitness_so_far;

};

} // namespace NeuroEvo

#endif
