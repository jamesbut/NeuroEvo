#ifndef _DOMAIN_H_
#define _DOMAIN_H_

/*
    Domain has a number of functions for evaluating populations
    and individuals on itself. It can do this in a serial or
    parallel way.
*/

#include <population.h>
#include <numeric>
#include <util/memory/shared_fitness_memory.h>
#include <optional>
#include <data/data_collection.h>

//TODO: Only include if found
#include <SFML/Graphics.hpp>

namespace NeuroEvo {

template <typename G>
class Domain {

public:

    Domain(const bool domain_trace, const double completion_fitness = 0.0, 
           const bool render = false, const unsigned int screen_width = 1280, 
           const unsigned int screen_height = 960) :
        _completion_fitness(completion_fitness),
        _domain_trace(domain_trace),
        _render(render),
        _screen_width(screen_width),
        _screen_height(screen_height),
        _window(initialise_sfml()),
        _complete(false) {}

    virtual ~Domain() = default;

    //Evaluate entire population each for a number of trials
    void evaluate_population(Population<G>& pop, const unsigned num_trials, const bool parallel) {

        std::vector<std::vector<double> > fitnesses;

        if(parallel)
            #ifdef __linux__
                fitnesses = evaluate_pop_parallel(pop, num_trials);
            #else
                fitnesses = evaluate_pop_serial(pop, num_trials);
            #endif
        else
            fitnesses = evaluate_pop_serial(pop, num_trials);

        //Get accumulated fitnesses using functor - eventually
        for(std::size_t i = 0; i < pop.get_size(); i++) {

            double average_fitness = std::accumulate(fitnesses.at(i).begin(), 
                                                     fitnesses.at(i).end(), 0.0) /
                                     fitnesses.at(i).size();

            pop.get_organisms().at(i).set_fitness(average_fitness);

        }

        //Check for domain completion
        if(!_complete)
            _complete = check_for_completion(pop);

    }

    //Evaluate individual for a number of trials
    double evaluate_org(Organism<G>& org, const unsigned num_trials) {

        std::vector<double> fitnesses(num_trials);

        for(unsigned int i = 0; i < num_trials; i++) {

            //Need to reset the network
            org.genesis();

            //Need to seed with random number
            fitnesses.at(i) = single_run(org, lrand48());
            std::cout << "Run: " << i << " Fitness: " << fitnesses.at(i) << std::endl;

        }

        return std::accumulate(fitnesses.begin(), fitnesses.end(), 0.0) / fitnesses.size();

    }

    virtual bool check_phenotype_spec(PhenotypeSpec<G>& pheno_spec) = 0;

    //Determines whether the current population
    //has 'completed' the domain, this normally
    //comes in the form of achieving a max fitness
    bool complete() { return _complete; };

protected:

    //This function is abstract and all domains should implement
    //one run for a single organism
    //This function is called by the evaluate functions
    //A random seed can be handed to this function in case there
    //is a random component to the domain and all individuals in the
    //population are required to be ran on the exact same domain.
    virtual double single_run(Organism<G>& org, unsigned rand_seed) = 0;

    //Checks domain for completion - can be overriden
    virtual bool check_for_completion(Population<G>& population) {

        std::vector<Organism<G>>& pop_orgs = population.get_organisms();

        for(std::size_t i = 0; i < pop_orgs.size(); i++)
            if(pop_orgs.at(i).get_fitness() >= _completion_fitness)
                return true;

        return false;

    }

    //All domains should implement how to render themselves
    virtual void render() = 0;

    //The fitness at which the domain is considered
    //solved
    const double _completion_fitness;

    //Bool determining whether domain is complete
    //this might involve more than just whether
    //some fitness has been reached
    bool _complete;

    const bool _domain_trace;

    //Rendering variables
    const bool _render;

    const int _screen_width;
    const int _screen_height;

    //SFML
    sf::RenderWindow _window;

private:

    std::vector<std::vector<double> > evaluate_pop_serial(Population<G>& pop,
                                                          const unsigned num_trials) {

        //Store fitnesses from runs
        std::vector<std::vector<double> > fitnesses(pop.get_size(), std::vector<double>(num_trials));

        std::vector<Organism<G>>& orgs = pop.get_organisms();

        for(unsigned int i = 0; i < num_trials; i++) {

            //Generate random number as the random seed
            //However each individual of the population gets the same seed
            //So they all generator the same random numbers for each run
            auto seed = lrand48();

            for(std::size_t j = 0; j < pop.get_size(); j++) {

                double fitness = single_run(orgs.at(j), seed);

                fitnesses.at(j).at(i) = fitness;

                orgs.at(j).genesis();

            }

        }

        return fitnesses;

    }

    //Only compile parallel execution if on linux
    //The implementation of this uses linux OS calls
#ifdef __linux__
    std::vector<std::vector<double> > evaluate_pop_parallel(Population<G>& pop,
                                                            const unsigned num_trials) {

        std::vector<std::vector<double> > fitnesses(pop.get_size(), std::vector<double>(num_trials));
        std::vector<Organism>& orgs = pop.get_organisms();

        //Check shared memory is available for parallel execution
        if(!_shared_fitness_mem.has_value())
            _shared_fitness_mem.emplace(Utils::SharedFitnessMemory(pop.get_size(), num_trials));

        auto concurrent_threads_supported = std::thread::hardware_concurrency();

        for(unsigned i = 0; i < num_trials; i++) {

            //Generate random number as the random seed
            //However each individual of the population gets the same seed
            //So they all generator the same random numbers for each run
            auto seed = lrand48();

            unsigned num_organisms_tested = 0;

            while(num_organisms_tested < orgs.size()) {

                //Do not spawn more threads than there are CPU cores on the machine
                unsigned num_organisms_left = orgs.size() - num_organisms_tested;
                unsigned num_threads_to_spawn = std::min(num_organisms_left, 
                                                         concurrent_threads_supported);

                for(unsigned j = 0; j < num_threads_to_spawn; j++) {

                    num_organisms_tested++;

                    //Spawn slaves
                    _slave_PIDs.push_back(::fork());

                    if(_slave_PIDs.back() == 0) {

                        double fitness = single_run(orgs.at(num_organisms_tested-1), seed);
                        _shared_fitness_mem->write_fitness(fitness, num_organisms_tested-1, i);

                        ::raise(SIGUSR1);

                    }

                }

                //Wait for all the slaves to finish
                int threads_left = num_threads_to_spawn;
                int slave_info;
                pid_t slave_PID;

                while(threads_left > 0) {

                    //Wait for next slave to finish
                    slave_PID = ::wait(&slave_info);

                    //Check for failure
                    if(slave_PID == -1) {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                    }

                    if(WIFSIGNALED(slave_info)) {

                        //If I didn't terminate slave, print out what did and exit
                        if(WTERMSIG(slave_info) != SIGUSR1) {

                            std::cerr << "Terminated with signal: " << WTERMSIG(slave_info) << 
                                std::endl;

                            //Kill all child processes
                            kill(_slave_PIDs.at(i), SIGKILL);

                            //Kill main process
                            exit(EXIT_FAILURE);

                        }

                    }

                    --threads_left;

                }

                _slave_PIDs.clear();

            }

            //Rebuild phenotypes for next run
            for(std::size_t j = 0; j < orgs.size(); j++)
                orgs.at(j).genesis();

        }

        //Get fitnesses from shared memory
        for(std::size_t i = 0; i < orgs.size(); i++)
            for(unsigned j = 0; j < num_trials; j++)
                fitnesses.at(i).at(j) = _shared_fitness_mem->get_fitness(i, j);

        return fitnesses;

    }
#endif

    //TODO: Should only be defined if SFML was found
    sf::RenderWindow initialise_sfml() {
        return sf::RenderWindow(sf::VideoMode(_screen_width, _screen_height), "Domain");
    }

    //Shared memory for parallel execution
    std::optional<Utils::SharedFitnessMemory> _shared_fitness_mem;

    //Slave IDs for parallel execution
    std::vector<pid_t> _slave_PIDs;

};

} // namespace NeuroEvo

#endif
