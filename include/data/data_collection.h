#ifndef _DATA_COLLECTION_H_
#define _DATA_COLLECTION_H_

/*
    This class collects data about the evolutionary
    run and prints it
*/

#include <algorithm>
#include <iomanip>
#include <ios>
#include <iostream>
#include <population.h>
#include <string>
#include <optional>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <util/formatting.h>

namespace NeuroEvo {

template <typename G, typename T>
class DataCollector 
{

public:

    DataCollector(const bool uuid_folders = true) :
        _uuid_folders(uuid_folders),
        _exp_dir_path(create_experiment_dir_path())
    {
        //Create experiment folder
        boost::filesystem::create_directory(_exp_dir_path);
    }

    void reset()
    {
        _run_dir_path.reset();
        _best_winner_so_far.reset();
    }

    void collect_generational_data(const Population<G, T>& population, const bool trace = true) 
    {
        //Create folder to store information if it has not already been created
        if(!_run_dir_path)
        {
            create_run_dir();
            _run_dir_paths.push_back(_run_dir_path.value());
        }

        //Generational winner
        const Organism<G, T>& gen_winner = calculate_generational_winner(population);
        save_generational_winner_to_file(gen_winner);

        //Best winner so far
        calculate_best_winner_so_far(gen_winner);
        save_best_winner_so_far_to_file();

        //Save entire population to file
        save_population_to_file(population);

        //Calculate population average fitness
        const double pop_average_fitness = calculate_population_avergage_fitness(population);

        if(trace) 
            print_info_to_screen(population, gen_winner.get_fitness(), pop_average_fitness);

    }

    const std::vector<std::string>& get_run_dir_paths() const
    {
        return _run_dir_paths;
    }

private:


    double calculate_population_avergage_fitness(const Population<G, T>& population) const 
    {
        double total_fitness = 0.;

        for(const auto& org : population.get_organisms())
            total_fitness += org.get_fitness();

        return total_fitness / (double)population.get_size();
    }

    const Organism<G, T>& calculate_generational_winner(const Population<G, T>& population) 
        const
    {
        const std::vector<Organism<G, T>>& orgs = population.get_organisms();

        //Find highest scoring organism in the population
        std::size_t gen_winner_index = 0;
        double gen_winner_fitness = orgs.at(gen_winner_index).get_fitness();

        for(std::size_t i = 1; i < population.get_size(); i++)
            if(orgs[i].get_fitness() > gen_winner_fitness) 
            {
                gen_winner_index = i;
                gen_winner_fitness = orgs[i].get_fitness();
            }

        return orgs[gen_winner_index];
    }

    void calculate_best_winner_so_far(const Organism<G, T>& gen_winner) 
    {
        //Check whether gen winner beats the highest score so far
        if(!_best_winner_so_far || gen_winner.get_fitness() > 
                _best_winner_so_far->get_fitness())
            _best_winner_so_far = gen_winner;
    }

    void save_generational_winner_to_file(const Organism<G, T>& gen_winner) const
    {
        //Print out highest scoring organism
        std::stringstream file_name;
        file_name << _run_dir_path.value() << "/generational_winner";

        gen_winner.save_org_to_file(file_name.str());
    }

    void save_best_winner_so_far_to_file() const
    {
        std::stringstream file_name;
        file_name << _run_dir_path.value() << "/best_winner_so_far";

        _best_winner_so_far->save_org_to_file(file_name.str());
    }

    void delete_exp_dir() const 
    {
        boost::filesystem::remove_all(_exp_dir_path);
    }

    void delete_run_dir() const 
    {
        boost::filesystem::remove_all(_run_dir_path.value());
    }

    const std::string& get_exp_dir_path() const
    { 
        return _exp_dir_path; 
    }
    
    const std::string& get_run_dir_path() const
    { 
        return _run_dir_path.value(); 
    }

    void save_population_to_file(const Population<G, T>& population) 
    {

        //Create and open file
        std::stringstream gen_file_name;
        gen_file_name << _run_dir_path.value() << "/population-gen_"
                    << std::to_string(population.get_gen_num());

        std::ofstream gen_file;
        gen_file.open(gen_file_name.str());

        //Write genome fitness and genome data to file
        for(const auto& org : population.get_organisms())
        {
            //Write fitness
            gen_file << org.get_fitness();

            gen_file << std::endl;
        }

        //Close file
        gen_file.close();

    }

    void print_info_to_screen(const Population<G, T>& population, 
                              const double gen_winner_fitness,
                              const double pop_average_fitness,
                              const unsigned print_header_every = 1e6)
    {
        const unsigned column_width = 16;

        //Print header titles every set amount of generations
        if((population.get_gen_num()-1) % print_header_every == 0)
        {
            const std::vector<std::string> header_titles{"Gen", "Best Winner", 
                                                        "Gen Winner", "Gen Average"};
            print_table_row(header_titles, column_width, true, true);
        }

        //Print population data
        const std::vector<double> row_data{(double)population.get_gen_num(),
                                           _best_winner_so_far->get_fitness(),
                                           gen_winner_fitness,
                                           pop_average_fitness};
        print_table_row(row_data, column_width);

    }

    void print_table_header(const std::vector<std::string>& header_titles, 
                            const unsigned column_width) const
    {
        print_table_row(header_titles, column_width, true, true);
    }

    const std::string create_experiment_dir_name() const
    {
        //Get experiment directory names
        std::vector<std::string> dir_names;

        for(const auto& entry : std::filesystem::directory_iterator(DATA_PATH))
        {
            //Split path by "/" delimeter and take the final string in that path
            std::vector<std::string> split_path;
            const std::string dir_path = entry.path();
            boost::split(split_path, dir_path, boost::is_any_of("/"));
            dir_names.push_back(split_path.back());
        }

        //Get previous experiment numbers from directory names
        std::vector<unsigned> exp_numbers;
        exp_numbers.reserve(dir_names.size());

        for(const auto& name : dir_names)
        {
            std::vector<std::string> split_string;
            boost::split(split_string, name, boost::is_any_of("_"));
            exp_numbers.push_back(std::stoi(split_string.back()));
        }

        //Find highest experiment number so far
        unsigned max_exp_num = 0;
        if(exp_numbers.size() != 0)
           max_exp_num = *std::max_element(exp_numbers.begin(), exp_numbers.end());

        //New folder name increments the exp number
        const std::string new_dir_name = "exp_" + std::to_string(max_exp_num+1);

        return new_dir_name;

    }

    const std::string create_experiment_dir_path() const
    {
        //First check to see whether data directory
        //exists and if not, create it
        if(!boost::filesystem::exists(DATA_PATH))
            boost::filesystem::create_directory(DATA_PATH);

        std::stringstream exp_dir_path;
        exp_dir_path << DATA_PATH << "/" << create_experiment_dir_name(); 
        return exp_dir_path.str();
    }

    //Create directory name for run
    const std::string create_run_dir_name() const
    {
        //Use a UUID for folder name or the date and time
        if(_uuid_folders) 
        {
            //Create a unique indentifier for the directory name
            boost::uuids::uuid uuid = boost::uuids::random_generator()();
            return boost::uuids::to_string(uuid);

        } else 
        {
            //TODO: Come back and fix this!
            //Get data and time for folder name
            /*
            boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();
            std::stringstream dir_name;
            dir_name << "population-" << timeLocal.date() << "-" << timeLocal.time_of_day(); 
            return dir_name.str();
            */
            return "population-cannot-calculate-current-time";

        }

    }

    void create_run_dir() 
    {
        const std::string run_dir_name = create_run_dir_name();
        
        //Create run directory
        std::stringstream run_dir_path;
        run_dir_path << _exp_dir_path << "/" << run_dir_name; 
        boost::filesystem::create_directory(run_dir_path.str());
        _run_dir_path = run_dir_path.str();

    }

    const bool _uuid_folders;

    const std::string _exp_dir_path;
    std::optional<std::string> _run_dir_path;
    //Store run direcory paths for later use
    std::vector<std::string> _run_dir_paths;

    std::optional<Organism<G, T>> _best_winner_so_far;

};

} // namespace NeuroEvo

#endif
