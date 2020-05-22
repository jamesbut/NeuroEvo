#ifndef _DATA_COLLECTION_H_
#define _DATA_COLLECTION_H_

/*
    This class collects data about the evolutionary
    run and prints it
*/

#include <population.h>
#include <string>
#include <optional>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace NeuroEvo {

template <typename G>
class DataCollector 
{

public:

    DataCollector(const bool uuid_folders = true) 
    {
    
        //First check to see whether data directory
        //exists and if not, create it
        if(!boost::filesystem::exists(DATA_PATH))
            boost::filesystem::create_directory(DATA_PATH);

        //Set up path
        std::stringstream folder_name;
        std::stringstream path;
        path << DATA_PATH << "/";

        //Use a UUID for folder name or the date and time
        if(uuid_folders) 
        {

            //Create a unique indentifier for the directory name
            boost::uuids::uuid uuid = boost::uuids::random_generator()();

            folder_name << path.str() << boost::uuids::to_string(uuid);

        } else 
        {

            folder_name << path.str() << "population-";

            //Get data and time for folder name
            boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();

            folder_name << timeLocal.date() << "-" << timeLocal.time_of_day();

        }

        _folder_path = folder_name.str();

        //Create population data folder
        boost::filesystem::create_directory(_folder_path);

    }

    void collect_generational_data(Population<G>& population, const bool trace = true) 
    {
        
        //Generational winner
        const Organism<G>& gen_winner = calculate_generational_winner(population);
        save_generational_winner_to_file(gen_winner);

        //Best winner so far
        calculate_best_winner_so_far(gen_winner);
        save_best_winner_so_far_to_file();

        //Save entire population to file
        save_population_to_file(population);

        if(trace)
            std::cout << "[Gen : " << population.get_gen_num()
                      << " | Best winner so far fitness: " << _best_winner_so_far->get_fitness()
                      << " | Gen winner fitness: " << gen_winner.get_fitness() << "]" << std::endl;

    }


private:

    const Organism<G>& calculate_generational_winner(const Population<G>& population) const 
    {
        const std::vector<Organism<G>>& orgs = population.get_organisms();

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

    void calculate_best_winner_so_far(const Organism<G>& gen_winner) 
    {
        //Check whether gen winner beats the highest score so far
        if(!_best_winner_so_far || gen_winner.get_fitness() > _best_winner_so_far->get_fitness())
            _best_winner_so_far = gen_winner;
    }

    void save_generational_winner_to_file(const Organism<G>& gen_winner) const
    {
        //Print out highest scoring organism
        std::stringstream file_name;
        file_name << _folder_path << "/generational_winner";

        gen_winner.save_org_to_file(file_name.str());
    }

    void save_best_winner_so_far_to_file() const
    {
        std::stringstream file_name;
        file_name << _folder_path << "/best_winner_so_far";

        _best_winner_so_far->save_org_to_file(file_name.str());
    }

    void delete_folder() const 
    {
        boost::filesystem::remove_all(_folder_path);
    }

    const std::string& get_folder_path() const
    { 
        return _folder_path; 
    };

    void save_population_to_file(Population<G>& population) 
    {

        //Create and open file
        std::stringstream gen_file_name;
        gen_file_name << _folder_path << "/population-gen_"
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

    //Path to folder in which to store
    //all data for a run
    std::string _folder_path;

    std::optional<Organism<G>> _best_winner_so_far;

};

} // namespace NeuroEvo

#endif
