#include <domains/domain.h>
#include <thread>
#include <sys/wait.h>

namespace NeuroEvo {
namespace Domains {

void Domain::evaluate_population(Population& pop, const unsigned NUM_TRIALS,
                                 const bool PARALLEL) {

    std::vector<std::vector<double> > fitnesses;

    if(PARALLEL)
        #ifdef __linux__
            fitnesses = evaluate_pop_parallel(pop, NUM_TRIALS);
        #else
            fitnesses = evaluate_pop_serial(pop, NUM_TRIALS);
        #endif
    else
        fitnesses = evaluate_pop_serial(pop, NUM_TRIALS);

    //Get accumulated fitnesses using functor - eventually
    for(std::size_t i = 0; i < pop.get_size(); i++) {

        double average_fitness = std::accumulate(fitnesses.at(i).begin(), fitnesses.at(i).end(), 0.0) /
                                 fitnesses.at(i).size();

        pop.get_organisms().at(i).set_fitness(average_fitness);

    }

    //Check for domain completion
    if(!_complete)
        _complete = check_for_completion(pop);

    //Print generational data
    data_collector.collect_generational_data(pop);

}

std::vector<std::vector<double> > Domain::evaluate_pop_serial(Population& pop,
                                                              const unsigned NUM_TRIALS) {

    //Store fitnesses from runs
    std::vector<std::vector<double> > fitnesses(pop.get_size(), std::vector<double>(NUM_TRIALS));

    std::vector<Organism>& orgs = pop.get_organisms();

    for(unsigned int i = 0; i < NUM_TRIALS; i++) {

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

#ifdef __linux__
std::vector<std::vector<double> > Domain::evaluate_pop_parallel(Population& pop,
                                                                const unsigned NUM_TRIALS) {

    std::vector<std::vector<double> > fitnesses(pop.get_size(), std::vector<double>(NUM_TRIALS));
    std::vector<Organism>& orgs = pop.get_organisms();

    //Check shared memory is available for parallel execution
    if(!_shared_fitness_mem.has_value())
        _shared_fitness_mem.emplace(Utils::SharedFitnessMemory(pop.get_size(), NUM_TRIALS));

    auto concurrent_threads_supported = std::thread::hardware_concurrency();

    for(unsigned i = 0; i < NUM_TRIALS; i++) {

        //Generate random number as the random seed
        //However each individual of the population gets the same seed
        //So they all generator the same random numbers for each run
        auto seed = lrand48();

        unsigned num_organisms_tested = 0;

        while(num_organisms_tested < orgs.size()) {

            //Do not spawn more threads than there are CPU cores on the machine
            unsigned num_organisms_left = orgs.size() - num_organisms_tested;
            unsigned num_threads_to_spawn = std::min(num_organisms_left, concurrent_threads_supported);

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

                        std::cerr << "Terminated with signal: " << WTERMSIG(slave_info) << std::endl;

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
        for(unsigned j = 0; j < NUM_TRIALS; j++)
            fitnesses.at(i).at(j) = _shared_fitness_mem->get_fitness(i, j);

    return fitnesses;

}
#endif

double Domain::evaluate_org(Organism& org, const unsigned NUM_TRIALS) {

    std::vector<double> fitnesses(NUM_TRIALS);

    for(unsigned int i = 0; i < NUM_TRIALS; i++) {

        //Need to reset the network
        org.genesis();

        //Need to seed with random number
        fitnesses.at(i) = single_run(org, lrand48());
        std::cout << "Run: " << i << " Fitness: " << fitnesses.at(i) << std::endl;

    }

    return std::accumulate(fitnesses.begin(), fitnesses.end(), 0.0) / fitnesses.size();

}

bool Domain::check_for_completion(Population& population) {

    std::vector<Organism>& pop_orgs = population.get_organisms();

    for(std::size_t i = 0; i < pop_orgs.size(); i++)
        if(pop_orgs.at(i).get_fitness() >= _COMPLETION_FITNESS)
            return true;

    return false;

}

} // namespace Domains
} // namespace NeuroEvo
