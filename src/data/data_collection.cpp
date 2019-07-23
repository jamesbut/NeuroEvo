#include <data/data_collection.h>
#include <sstream>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

DataCollector::DataCollector(const bool UUID_FOLDERS) {

    //First check to see whether data directory
    //exists and if not, create it
    if(!boost::filesystem::exists("../../data"))
        boost::filesystem::create_directory("../../data");

    //Set up path
    std::stringstream folder_name;
    std::stringstream path;
    path << "../../data/";

    //Use a UUID for folder name or the date and time
    if(UUID_FOLDERS) {

        //Create a unique indentifier for the directory name
        boost::uuids::uuid uuid = boost::uuids::random_generator()();

        folder_name << path.str() << boost::uuids::to_string(uuid);

    } else {

        folder_name << path.str() << "population-";

        //Get data and time for folder name
        boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();

        folder_name << timeLocal.date() << "-" << timeLocal.time_of_day();

    }

    _folder_path = folder_name.str();

    //Create population data folder
    boost::filesystem::create_directory(_folder_path);

}

void DataCollector::collect_generational_data(Population& population) {

    print_population_to_file(population);
    print_generational_winner(population);
    print_best_winner_so_far(population);

}

void DataCollector::print_population_to_file(Population& population) {

    //Create and open file
    std::stringstream gen_file_name;
    gen_file_name << _folder_path << "/population-gen_"
                  << std::to_string(population.get_gen_num());

    std::ofstream gen_file;
    gen_file.open(gen_file_name.str());

    //Write genome fitness and genome data
    std::vector<Organism>& pop_orgs = population.get_organisms();

    for(unsigned int i = 0; i < population.get_size(); i++) {

        Organism& org = pop_orgs.at(i);
        Genotype& genotype = org.get_genotype();

        //Write fitness
        gen_file << org.get_fitness();

        //Write genome
        //for(unsigned int j = 0; j < genotype.get_genes().size(); j++)
        //  gen_file << "," << genotype.get_genes().at(j);

        gen_file << std::endl;

    }

    //Close file
    gen_file.close();

}

void DataCollector::print_population_winner(Population& population) {

    std::vector<Organism>& orgs = population.get_organisms();

    //Find highest scoring organism in the population
    double largest_fitness = orgs.at(0).get_fitness();
    int best_org = 0;

    for(std::size_t i = 1; i < population.get_size(); i++)
        if(orgs.at(i).get_fitness() > largest_fitness) {
            largest_fitness = orgs.at(i).get_fitness();
            best_org = i;
        }

    //Print out highest scoring organism
    std::stringstream file_name;
    file_name << _folder_path << "/final_pop_winner";

    orgs.at(best_org).print_org(file_name.str());

    std::cout << "Final pop winner printed" << std::endl;

}

void DataCollector::print_generational_winner(Population& population) {

    std::vector<Organism>& orgs = population.get_organisms();

    //Find highest scoring organism in the population
    double largest_fitness = orgs.at(0).get_fitness();
    int best_org = 0;

    for(std::size_t i = 1; i < population.get_size(); i++)
        if(orgs.at(i).get_fitness() > largest_fitness) {
            largest_fitness = orgs.at(i).get_fitness();
            best_org = i;
        }

    //Print out highest scoring organism
    std::stringstream file_name;
    file_name << _folder_path << "/generational_winner";

    orgs.at(best_org).print_org(file_name.str());

}

void DataCollector::print_best_winner_so_far(Population& population) {

    std::vector<Organism>& orgs = population.get_organisms();

    //Find highest scoring organism in the population
    double largest_fitness = orgs.at(0).get_fitness();
    int best_org = 0;

    for(std::size_t i = 1; i < population.get_size(); i++)
        if(orgs.at(i).get_fitness() > largest_fitness) {
            largest_fitness = orgs.at(i).get_fitness();
            best_org = i;
        }

    //Check whether they beat the highest score so far
    if(!best_fitness_so_far.has_value())
        best_fitness_so_far = largest_fitness;
    else {

        if(largest_fitness > best_fitness_so_far)
            best_fitness_so_far = largest_fitness;
        else
            //Don't print if the best fitness so far has not been beaten
            return;

    }

    //Print out highest scoring organism
    std::stringstream file_name;
    file_name << _folder_path << "/best_winner_so_far";

    orgs.at(best_org).print_org(file_name.str());

}

void DataCollector::delete_folder() {

    boost::filesystem::remove_all(_folder_path);

}
