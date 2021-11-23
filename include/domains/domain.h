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
#include <util/statistics/distributions/uniform_unsigned_distribution.h>
#include <mutex>

#if SFML_FOUND
#include <SFML/Graphics.hpp>
#endif

namespace NeuroEvo {

inline std::mutex mtx;

template <typename G, typename T>
class Domain
{

public:

    Domain(const bool domain_trace, const double completion_fitness = 0.0,
           const std::optional<unsigned> seed = std::nullopt,
           const bool render = false, const unsigned int screen_width = 1280,
           const unsigned int screen_height = 960) :
        _completion_fitness(completion_fitness),
        _complete(false),
        _domain_trace(domain_trace),
        _seed(seed),
        _trial_seed_sequence(seed),
        _render(render),
        _screen_width(screen_width),
        _screen_height(screen_height)
    {
#if SFML_FOUND
        if(_render)
            _window.create(sf::VideoMode(_screen_width, _screen_height), "Domain");
#endif
        if(_seed.has_value())
            _run_seed_sequence.set_seed(_seed.value());
    }

    Domain(const Domain& domain) :
        _completion_fitness(domain._completion_fitness),
        _complete(domain._complete),
        _domain_trace(domain._domain_trace),
        _seed(domain._seed),
        _trial_seed_sequence(domain._trial_seed_sequence),
        _render(domain._render),
        _screen_width(domain._screen_width),
        _screen_height(domain._screen_height),
        _domain_hyperparams(domain._domain_hyperparams)
    {
#if SFML_FOUND
        if(_render)
            _window.create(sf::VideoMode(_screen_width, _screen_height), "Domain");
#endif
    }

    Domain(const JSON& json) :
        _completion_fitness(json.at({"completion_fitness"})),
        _complete(false),
        _domain_trace(json.at({"trace"})),
        _seed(json.at({"seed"})),
        _trial_seed_sequence(json.at({"seed"})),
        _render(json.at({"render"})),
        _screen_width(1280),
        _screen_height(960) {}

    virtual ~Domain() = default;

    //Evaluate entire population each for a number of trials
    void evaluate_population(Population<G, T>& pop, const unsigned num_trials,
                             const bool parallel)
    {

        std::vector<std::vector<double> > fitnesses;

        if(parallel)
            //#ifdef __linux__
            //    fitnesses = evaluate_pop_parallel(pop, num_trials);
            //#else
                fitnesses = evaluate_pop_serial(pop, num_trials);
            //#endif
        else
            fitnesses = evaluate_pop_serial(pop, num_trials);

        //Get accumulated fitnesses using functor - eventually
        for(std::size_t i = 0; i < pop.get_size(); i++) {

            double average_fitness = std::accumulate(fitnesses.at(i).begin(),
                                                     fitnesses.at(i).end(), 0.0) /
                                     fitnesses.at(i).size();

            pop.set_organism_fitness(i, average_fitness, _completion_fitness);

        }

        //Check for domain completion
        if(!_complete)
            _complete = check_for_completion(pop);

    }

    //Evaluate individual for a number of trials
    double evaluate_org(Organism<G, T>& org, const unsigned num_trials = 1,
                        const bool verbosity = true)
    {

        std::vector<double> fitnesses(num_trials);

        for(unsigned i = 0; i < num_trials; i++)
        {

            //Need to reset the network
            org.genesis();

            const auto trial_seed = _trial_seed_sequence.next();
            trial_reset(i);

            fitnesses.at(i) = single_run(org, trial_seed);

            if(verbosity)
                std::cout << "Run: " << i << " Fitness: " << fitnesses.at(i)
                    << std::endl;

        }

        return std::accumulate(fitnesses.begin(), fitnesses.end(), 0.0)
                               / fitnesses.size();

    }

    virtual bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const = 0;

    //Determines whether the current population
    //has 'completed' the domain, this normally
    //comes in the form of achieving a max fitness
    bool complete() const
    {
        return _complete;
    };

    void exp_run_reset(const unsigned run_num)
    {
        _complete = false;

        if(_seed.has_value())
            _trial_seed_sequence.set_seed(_seed.value());
        else
            _trial_seed_sequence.randomly_seed();

        mtx.lock();
        exp_run_reset_impl(run_num, _run_seed_sequence.next());
        mtx.unlock();
    }

    double get_completion_fitness() const
    {
        return _completion_fitness;
    }

    auto clone() const
    {
        return std::unique_ptr<Domain>(clone_impl());
    }

    void set_render(const bool render)
    {
        _render = render;
#if SFML_FOUND
        if(_render)
            _window.create(sf::VideoMode(_screen_width, _screen_height), "Domain");
#endif
    }

    void set_trace(const bool trace)
    {
        _domain_trace = trace;
    }

    void set_seed(const std::optional<const unsigned>& seed)
    {
        _seed = seed;
        if(_seed.has_value())
        {
            _trial_seed_sequence.set_seed(_seed.value());
            _run_seed_sequence.set_seed(_seed.value());
        }
    }

    std::optional<std::vector<double>> get_hyperparams() const
    {
        return _domain_hyperparams;
    }

protected:

    //This function is abstract and all domains should implement
    //one run for a single organism
    //This function is called by the evaluate functions
    //A random seed can be handed to this function in case there
    //is a random component to the domain and all individuals in the
    //population are required to be ran on the exact same domain.
    virtual double single_run(Organism<G, T>& org, unsigned rand_seed) = 0;

    //Checks domain for completion - can be overriden
    virtual bool check_for_completion(Population<G, T>& population)
    {
        for(const auto& org : population.get_organisms())
            if(org.is_domain_winner())
                return true;

        return false;
    }

    virtual Domain* clone_impl() const = 0;

    //All domains should implement how to render themselves
    virtual void render() = 0;

    //All domains should implement how they are reset
    //I actually think these do not need to be abstract, it is not essential
    //that they have implementations

    //Reset after each experiment
    virtual void exp_run_reset_impl(const unsigned run_num,
                                    const unsigned run_seed) = 0;
    //Reset after each trial
    virtual void trial_reset(const unsigned trial_num) = 0;
    //Reset after each organism is evaluated
    virtual void org_reset() {}

    //Set domain hyperparameters
    void set_hyperparams(const std::vector<double>& hyperparams)
    {
        _domain_hyperparams = hyperparams;
    }

    //The fitness at which the domain is considered
    //solved
    const double _completion_fitness;

    //Bool determining whether domain is complete
    //this might involve more than just whether
    //some fitness has been reached
    bool _complete;

    bool _domain_trace;

    std::optional<unsigned> _seed;
    //We need a sequence of seeds to hand to each single run
    //This sequence of seeds is seeded with _seed... sorry :/
    UniformUnsignedDistribution _trial_seed_sequence;
    inline static UniformUnsignedDistribution _run_seed_sequence;

    //Rendering variables
    bool _render;

    const int _screen_width;
    const int _screen_height;

#if SFML_FOUND
    sf::RenderWindow _window;
#endif

private:

    std::vector<std::vector<double> > evaluate_pop_serial(Population<G, T>& pop,
                                                          const unsigned num_trials)
    {

        //Store fitnesses from runs
        std::vector<std::vector<double> > fitnesses(pop.get_size(),
                                                    std::vector<double>(num_trials));

        for(unsigned i = 0; i < num_trials; i++)
        {
            //Each individual in the population has the same seed for each trial
            const auto seed = _trial_seed_sequence.next();
            trial_reset(i);

            for(std::size_t j = 0; j < pop.get_size(); j++)
            {
                org_reset();
                const double fitness = single_run(pop.get_mutable_organism(j), seed);
                fitnesses[j][i] = fitness;
                pop.organism_genesis(j);
            }

        }

        return fitnesses;

    }

/*
    //Only compile parallel execution if on linux
    //The implementation of this uses linux OS calls
#ifdef __linux__
    std::vector<std::vector<double> > evaluate_pop_parallel(Population<G, T>& pop,
                                                            const unsigned num_trials) {

        std::vector<std::vector<double> > fitnesses(pop.get_size(),
                                                    std::vector<double>(num_trials));
        std::vector<Organism<G, T>>& orgs = pop.get_organisms();

        //Check shared memory is available for parallel execution
        if(!_shared_fitness_mem.has_value())
            _shared_fitness_mem.emplace(Utils::SharedFitnessMemory(pop.get_size(), num_trials));

        auto concurrent_threads_supported = std::thread::hardware_concurrency();

        for(unsigned i = 0; i < num_trials; i++) {

            const auto seed = _seed_sequence.next();

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
*/

    //Shared memory for parallel execution
    std::optional<SharedFitnessMemory> _shared_fitness_mem;

    //Slave IDs for parallel execution
    std::vector<pid_t> _slave_PIDs;


    //Domain hyperparameters
    std::optional<std::vector<double>> _domain_hyperparams;

};

} // namespace NeuroEvo

#endif
