#ifndef _OPTIMISER_H_
#define _OPTIMISER_H_

/*
    Specifies an optimiser to create a new population in the evolutionary algorithm.
*/

#include <population.h>
#include <domains/domain.h>
#include <data/data_collection.h>

namespace NeuroEvo {

template <typename G, typename T>
class Optimiser
{

public:

    Optimiser(const unsigned num_genes,
              const unsigned max_gens,
              const unsigned pop_size,
              const bool quit_when_domain_complete = true,
              const unsigned num_trials = 1,
              const std::optional<unsigned> seed = std::nullopt) :
        _num_genes(num_genes),
        _max_gens(max_gens),
        _pop_size(pop_size),
        _num_trials(num_trials),
        _seed(seed),
        _trace(false),
        _quit_when_domain_complete(quit_when_domain_complete) {}

    Optimiser(const Optimiser& optimiser) :
        _num_genes(optimiser._num_genes),
        _max_gens(optimiser._max_gens),
        _pop_size(optimiser._pop_size),
        _num_trials(optimiser._num_trials),
        _seed(optimiser._seed),
        _trace(optimiser._trace),
        _quit_when_domain_complete(optimiser._quit_when_domain_complete) {}

    virtual ~Optimiser() = default;

    //Optimise according to optimiser algorithm
    //Returns bool to indicate whether all domains were solved or not
    bool optimise(std::vector<std::unique_ptr<Domain<G, T>>>& domains,
                  std::shared_ptr<GPMap<G, T>> gp_map,
                  DataCollector<G, T>& data_collector) {

        _population = initialise_population(gp_map);
        
        // Set whether organism is a winner based on the average domain 
        // completion fitness.
        // This is not desirable, I want to change this.
        const double average_domain_completion_fitness = 
            std::accumulate(
                domains.begin(),
                domains.end(),
                0.0,
                [](const double sum_so_far,
                   const std::unique_ptr<Domain<G, T>>& domain)
                {return sum_so_far + domain->get_completion_fitness();}
            ) / domains.size();

        bool finished = false;
        unsigned gen = 1;

        do
        {
            //Evaluate population on domains
            evaluate_population(
                _population, domains, _num_trials,
                average_domain_completion_fitness
            );

            //Check for completion
            finished = optimisation_finished(gen, domains);

            //Collect data
            data_collector.collect_generational_data(
                _population, gen, finished, domains
            );

            if(finished) break;

            //Optimiser step - creates next generation population
            _population = step(gp_map);

            gen++;

        } while (!finished);

        _finished_gen = gen;

        /*
        std::cout << "Best org:" << std::endl;
        const auto best_org = _population.get_fittest_org();
        std::cout << best_org << std::endl;
        */

        unsigned num_domains_completed = 0;
        for(const auto& domain : domains)
            if(domain->complete())
                num_domains_completed++;
        if(num_domains_completed == domains.size())
            return true;

        return false;

    }

    unsigned get_max_gens() const
    {
        return _max_gens;
    }

    unsigned get_finished_gen() const
    {
        return _finished_gen.value();
    }

    void seed_optimiser(const unsigned seed)
    {
        _seed = seed;
    }

    void set_trace(const bool trace)
    {
        _trace = trace;
    }

    auto clone() const
    {
        return std::unique_ptr<Optimiser>(clone_impl());
    }

    //Reset optimiser
    virtual void reset() = 0;

protected:

    //Applies evolutionary operators and creates new population
    //This is different for each optimiser
    virtual Population<G, T> step(std::shared_ptr<GPMap<G, T>> gp_map) = 0;

    //Each algorithm will intialise its population differently - 
    //implement that in here
    virtual Population<G, T> initialise_population(
        std::shared_ptr<GPMap<G, T>> gp_map) = 0;

    virtual Optimiser* clone_impl() const = 0;

    const unsigned _num_genes;
    const unsigned _max_gens;
    const unsigned _pop_size;
    const unsigned _num_trials;

    std::optional<unsigned> _finished_gen;

    Population<G, T> _population;

    std::optional<unsigned> _seed;

    bool _trace;

private:

    void evaluate_population(
        Population<G, T>& population, 
        std::vector<std::unique_ptr<Domain<G, T>>>& domains,
        const unsigned num_trials,
        const double average_domain_completion_fitness
    )
    {
        for(std::size_t i = 0; i < population.get_size(); i++)
        {
            double total_fitness = 0.0;

            for(std::size_t j = 0; j < domains.size(); j++)
                total_fitness += domains[j]->evaluate(
                    population.get_mutable_organism(i)
                );    

            const double average_fitness = total_fitness / domains.size();

            population.set_organism_fitness(
                i, average_fitness, average_domain_completion_fitness
            );
        }

        // Checks each domain for completion
        // TODO: Do not know whether the functionality of this works now that 
        // we have a list of domains - check!
        for(auto& domain : domains)
            domain->set_complete(domain->check_for_completion(population));
    }

    bool optimisation_finished(
        const unsigned curr_gen,
        const std::vector<std::unique_ptr<Domain<G, T>>>& domains
    ) const
    {
        if(curr_gen >= _max_gens)
            return true;

        // Return true if all domains are completed
        unsigned num_domains_completed = 0;
        for(const auto& domain : domains)
            if(domain->complete() && _quit_when_domain_complete)
                num_domains_completed++;
        if(num_domains_completed == domains.size())
            return true;

        return false;
    }

    const bool _quit_when_domain_complete;

};

} // namespace NeuroEvo

#endif
