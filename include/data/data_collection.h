#ifndef _DATA_COLLECTION_H_
#define _DATA_COLLECTION_H_

/*
    This class collects data about the evolutionary
    run and prints it
*/

#include <population.h>
#include <string>
#include <optional>

class DataCollector {

public:

    DataCollector(const bool UUID_FOLDERS = true);

    void collect_generational_data(Population& population);

    void print_population_winner(Population& population);
    void print_generational_winner(Population& population);
    void print_best_winner_so_far(Population& population);

    void delete_folder();

    const std::string get_folder_path() { return _folder_path; };

private:

    void print_population_to_file(Population& population);

    //Path to folder in which to store
    //all data for a run
    std::string _folder_path;

    std::optional<double> best_fitness_so_far;

};

#endif
