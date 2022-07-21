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
#include <boost/algorithm/string.hpp>
#include <vector>
#include <util/formatting.h>
#include <util/statistics/statistic.h>
#include <filesystem>
#include <domains/domain.h>

namespace NeuroEvo {

template <typename G, typename T>
class DataCollector
{

public:

    //If one does not want to dump the data then just provide a nullopt to the
    //constructor
    DataCollector(const std::optional<std::string>& exp_dir_path,
                  const unsigned max_gens,
                  const bool dump_winners_only, const bool trace) :
        _uuid_folders(true),
        _exp_dir_path(exp_dir_path),
        _max_gens(max_gens),
        _dump_winners_only(dump_winners_only),
        _trace(trace)
    {
        //We can reserve up to max gens for member vectors so that they never have
        //to resize
        _mean_gen_fitnesses.reserve(max_gens);
        _median_gen_fitnesses.reserve(max_gens);
        _lq_gen_fitnesses.reserve(max_gens);
        _uq_gen_fitnesses.reserve(max_gens);
        _best_so_far_fitnesses.reserve(max_gens);
    }

    void reset()
    {
        _run_dir_path.reset();
        _best_winner_so_far.reset();
    }

    void collect_generational_data(
        const Population<G, T>& population,
        const unsigned current_gen,
        const bool final_gen,
        const std::unique_ptr<Domain<G, T>>& domain)
    {
        //Create folder to store information if it has not already been created
        if(!_run_dir_path && _exp_dir_path.has_value())
        {
            create_run_dir();
            _run_dir_paths.push_back(_run_dir_path.value());
        }

        //Generational winner
        const Organism<G, T>& gen_winner = calculate_generational_winner(population);
        if(_exp_dir_path.has_value())
            save_generational_winner_to_file(gen_winner, domain);

        //Best winner so far
        calculate_best_winner_so_far(gen_winner);
        if(_exp_dir_path.has_value())
            save_best_winner_so_far_to_file(domain);

        calculate_population_statistics(population);
        if(final_gen)
            complete_statistics();

        //Save entire population to file
        if(_exp_dir_path.has_value())
            save_population_to_file(population, final_gen);

        //If _dump_winners_only is true delete the entire run directory if the
        //best winner fitness is not a domain winner
        if(_dump_winners_only && final_gen)
            if(!_best_winner_so_far.value().is_domain_winner())
                std::filesystem::remove_all(_run_dir_path.value());

        if(_trace)
            print_info_to_screen(population, current_gen,
                                 gen_winner.get_fitness().value(),
                                 _mean_gen_fitnesses.back());

    }

    const std::vector<std::string>& get_run_dir_paths() const
    {
        return _run_dir_paths;
    }

    //Creates experiment directory and returns path.
    //If a JSON config is given the directory is calculated from the values 
    //in that.
    //This path is more often than not given to the object constructor.
    static const std::string create_exp_dir(
        const std::optional<JSON>& config = std::nullopt
    )
    {
        // Create path name
        std::string exp_dir_path;
        if(config.has_value() && 
           config->has_value({"Logging", "exp_dir_name"}))
            exp_dir_path = 
                config->get<const std::string>({"Logging", "data_dir_path"}) +
                config->get<const std::string>({"Logging", "exp_dir_name"});
        else
            exp_dir_path = create_experiment_dir_path();

        // Create directory
        if(!boost::filesystem::exists(exp_dir_path))
            boost::filesystem::create_directories(exp_dir_path);
        return exp_dir_path;
    }

private:

    void calculate_population_statistics(const Population<G, T>& population)
    {
        const std::vector<double> fitnesses = population.get_fitnesses();

        _mean_gen_fitnesses.push_back(calculate_mean(fitnesses));
        _median_gen_fitnesses.push_back(calculate_median(fitnesses));
        _lq_gen_fitnesses.push_back(calculate_quantile(fitnesses, 0.25));
        _uq_gen_fitnesses.push_back(calculate_quantile(fitnesses, 0.75));
    }

    //Fills the remainder of the statistics up to max gens if the run was finished early
    //This helps with the python plots
    void complete_statistics()
    {
        while(_mean_gen_fitnesses.size() < _max_gens)
        {
            _mean_gen_fitnesses.push_back(_mean_gen_fitnesses.back());
            _median_gen_fitnesses.push_back(_median_gen_fitnesses.back());
            _lq_gen_fitnesses.push_back(_lq_gen_fitnesses.back());
            _uq_gen_fitnesses.push_back(_uq_gen_fitnesses.back());
            _best_so_far_fitnesses.push_back(_best_so_far_fitnesses.back());
        }
    }

    double calculate_population_avergage_fitness(const Population<G, T>& population)
        const
    {
        double total_fitness = 0.;

        for(const auto& org : population.get_organisms())
            total_fitness += org.get_fitness();

        return total_fitness / (double)population.get_size();
    }

    const Organism<G, T>& calculate_generational_winner(
        const Population<G, T>& population) const
    {
        const std::vector<Organism<G, T>>& orgs = population.get_organisms();

        //If one of the organisms is a domain winner, this is immediately returned
        //regardless of fitness. Sometimes an organism can be a domain winner and not
        //have the highest fitness. The domain winners take precedent.
        for(const auto& org : orgs)
            if(org.is_domain_winner())
                return org;

        //Find highest scoring organism in the population
        std::size_t gen_winner_index = 0;
        double gen_winner_fitness = orgs.at(gen_winner_index).get_fitness().value();

        for(std::size_t i = 1; i < population.get_size(); i++)
            if(orgs[i].get_fitness() > gen_winner_fitness)
            {
                gen_winner_index = i;
                gen_winner_fitness = orgs[i].get_fitness().value();
            }

        return orgs[gen_winner_index];
    }

    void calculate_best_winner_so_far(const Organism<G, T>& gen_winner)
    {
        //Check whether gen winner beats the highest score so far
        if(!_best_winner_so_far ||
           gen_winner.get_fitness() > _best_winner_so_far->get_fitness())
            _best_winner_so_far = gen_winner;

        //Save best winner so far fitness
        _best_so_far_fitnesses.push_back(_best_winner_so_far->get_fitness().value());
    }

    void save_generational_winner_to_file(const Organism<G, T>& gen_winner,
                                          const std::unique_ptr<Domain<G, T>>& domain)
        const
    {
        //Print out highest scoring organism
        JSON json;
        json.emplace("Organism", gen_winner.to_json());
        json.emplace("Domain", domain->to_json());
        json.save_to_file(_run_dir_path.value() + "/generational_winner");
    }

    //Pass in domain reference so the domain used to train the organism is saved also
    void save_best_winner_so_far_to_file(const std::unique_ptr<Domain<G, T>>& domain)
        const
    {
        std::stringstream file_path;
        file_path << _run_dir_path.value() << "/best_winner_so_far";
        JSON json;
        json.emplace("Organism", _best_winner_so_far->to_json());
        json.emplace("Domain", domain->to_json());
        json.save_to_file(file_path.str());
    }

    void delete_exp_dir() const
    {
        if(_exp_dir_path.has_value())
            boost::filesystem::remove_all(_exp_dir_path.value());
    }

    void delete_run_dir() const
    {
        boost::filesystem::remove_all(_run_dir_path.value());
    }

    const std::optional<const std::string>& get_exp_dir_path() const
    {
        return _exp_dir_path;
    }

    const std::string& get_run_dir_path() const
    {
        return _run_dir_path.value();
    }

    void save_population_to_file(const Population<G, T>& population,
                                 const bool final_gen)
    {

        //The statistics files seem to be enough information to store for now
        /*
        //Create and open file
        std::stringstream gen_file_name;
        gen_file_name << _run_dir_path.value() << "/population-gen_"
                    << std::to_string(population.get_gen_num());

        std::ofstream gen_file;
        gen_file.open(gen_file_name.str());

        //Write genome fitness to file
        for(const auto& org : population.get_organisms())
        {
            //Write fitness
            gen_file << org.get_fitness();
            gen_file << std::endl;
        }

        //Close file
        gen_file.close();
        */

        //If final gen, dump population statistics from over all the generations
        if(final_gen)
            save_population_statistics_to_file();

    }

    void save_population_statistics_to_file() const
    {
        //Fitness means
        const std::string mean_fitnesses_file_name = _run_dir_path.value()
            + "/mean_fitnesses";
        std::ofstream mean_fitnesses_file(mean_fitnesses_file_name);

        for(const auto& fitness : _mean_gen_fitnesses)
            mean_fitnesses_file << fitness << std::endl;

        mean_fitnesses_file.close();

        //Fitness medians
        const std::string median_fitnesses_file_name = _run_dir_path.value()
            + "/median_fitnesses";
        std::ofstream median_fitnesses_file(median_fitnesses_file_name);

        for(const auto& fitness : _median_gen_fitnesses)
            median_fitnesses_file << fitness << std::endl;

        median_fitnesses_file.close();

        //Fitness lower quantiles
        const std::string lq_fitnesses_file_name =
            _run_dir_path.value() + "/lq_fitnesses";
        std::ofstream lq_fitnesses_file(lq_fitnesses_file_name);

        for(const auto& fitness : _lq_gen_fitnesses)
            lq_fitnesses_file << fitness << std::endl;

        lq_fitnesses_file.close();

        //Fitness upper quantiles
        const std::string uq_fitnesses_file_name =
            _run_dir_path.value() + "/uq_fitnesses";
        std::ofstream uq_fitnesses_file(uq_fitnesses_file_name);

        for(const auto& fitness : _uq_gen_fitnesses)
            uq_fitnesses_file << fitness << std::endl;

        uq_fitnesses_file.close();

        //Best fitnesses so far
        const std::string best_fitnesses_file_name = _run_dir_path.value()
            + "/best_fitnesses";
        std::ofstream best_fitnesses_file(best_fitnesses_file_name);

        for(const auto& fitness : _best_so_far_fitnesses)
            best_fitnesses_file << fitness << std::endl;

        best_fitnesses_file.close();
    }

    void print_info_to_screen(const Population<G, T>& population,
                              const unsigned current_gen,
                              const double gen_winner_fitness,
                              const double pop_average_fitness,
                              const unsigned print_header_every = 1e6)
    {
        const unsigned column_width = 16;

        //Print header titles every set amount of generations
        if((current_gen-1) % print_header_every == 0)
        {
            const std::vector<std::string> header_titles{"Gen", "Best Winner",
                                                         "Gen Winner", "Gen Average"};
            print_table_row(header_titles, column_width, true, true);
        }

        //Print population data
        const std::vector<double> row_data{static_cast<double>(current_gen),
                                           _best_winner_so_far->get_fitness().value(),
                                           gen_winner_fitness,
                                           pop_average_fitness};
        print_table_row(row_data, column_width);

    }

    void print_table_header(const std::vector<std::string>& header_titles,
                            const unsigned column_width) const
    {
        print_table_row(header_titles, column_width, true, true);
    }

    static const std::string create_experiment_dir_name()
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

            //Just captures the exception where string is not integer
            try
            {
                exp_numbers.push_back(std::stoi(split_string.back()));
            }
            catch (...) {}
        }

        //Find highest experiment number so far
        unsigned max_exp_num = 0;
        if(exp_numbers.size() != 0)
           max_exp_num = *std::max_element(exp_numbers.begin(), exp_numbers.end());

        //New folder name increments the exp number
        const std::string new_dir_name = "exp_" + std::to_string(max_exp_num+1);

        return new_dir_name;

    }

    static const std::string create_experiment_dir_path()
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
            boost::posix_time::ptime timeLocal =
                boost::posix_time::second_clock::local_time();
            std::stringstream dir_name;
            dir_name << "population-" << timeLocal.date() << "-" <<
                timeLocal.time_of_day();
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
        run_dir_path << _exp_dir_path.value() << "/" << run_dir_name;
        boost::filesystem::create_directory(run_dir_path.str());
        _run_dir_path = run_dir_path.str();

    }

    const bool _uuid_folders;

    const std::optional<const std::string> _exp_dir_path;
    std::optional<std::string> _run_dir_path;
    //Store run direcory paths for later use
    std::vector<std::string> _run_dir_paths;

    std::optional<Organism<G, T>> _best_winner_so_far;

    const unsigned _max_gens;

    const bool _dump_winners_only;

    const bool _trace;

    //Data structures to store data over the generations
    std::vector<double> _mean_gen_fitnesses;
    std::vector<double> _median_gen_fitnesses;
    std::vector<double> _lq_gen_fitnesses;
    std::vector<double> _uq_gen_fitnesses;

    std::vector<double> _best_so_far_fitnesses;

};

} // namespace NeuroEvo

#endif
