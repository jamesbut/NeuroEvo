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

namespace NeuroEvo {
namespace Domains {

class Domain {

public:

    Domain(const bool DOMAIN_TRACE, const double COMPLETION_FITNESS = 0.0) :
        _COMPLETION_FITNESS(COMPLETION_FITNESS),
        _DOMAIN_TRACE(DOMAIN_TRACE),
        _complete(false) {};

    //Evaluate entire population each for a number of trials
    void evaluate_population(Population& pop, const unsigned NUM_TRIALS, const bool PARALLEL);

    //Evaluate individual for a number of trials
    double evaluate_org(Organism& org, const unsigned NUM_TRIALS);

    virtual bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) = 0;

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
    virtual double single_run(Organism& org, unsigned rand_seed) = 0;

    //Checks domain for completion - can be overriden
    virtual bool check_for_completion(Population& population);

    //The fitness at which the domain is considered
    //solved
    const double _COMPLETION_FITNESS;

    //Bool determining whether domain is complete
    //this might involve more than just whether
    //some fitness has been reached
    bool _complete;

    const bool _DOMAIN_TRACE;

private:

    std::vector<std::vector<double> > evaluate_pop_serial(Population& pop,
                                                          const unsigned NUM_TRIALS);

    //Only compile parallel execution if on linux
    //The implementation of this uses linux OS calls
#ifdef __linux__
    std::vector<std::vector<double> > evaluate_pop_parallel(Population& pop,
                                                            const unsigned NUM_TRIALS);
#endif

    //Shared memory for parallel execution
    std::optional<Utils::SharedFitnessMemory> _shared_fitness_mem;

    //Slave IDs for parallel execution
    std::vector<pid_t> _slave_PIDs;

    DataCollector data_collector;

};

} // namespace Domains
} // namespace NeuroEvo

#endif
