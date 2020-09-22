#ifndef _OPTIMISER_H_
#define _OPTIMISER_H_

/*
    Specifies an optimiser to create a new population in the evolutionary algorithm.
*/

#include <population.h>
#include <domains/domain.h>

namespace NeuroEvo {

template <typename G, typename T>
class Optimiser
{

public:

    Optimiser(GPMap<G, T>& gp_map,
              const unsigned num_genes,
              const unsigned max_gens,
              const unsigned pop_size,
              const unsigned num_trials = 1) :
        _gp_map(gp_map),
        _num_genes(num_genes),
        _max_gens(max_gens),
        _pop_size(pop_size), 
        _num_trials(num_trials) {}

    virtual ~Optimiser() = default;

    //Optimise according to optimiser algorithm
    //Returns bool to indicate whether domain was solved or not
    bool optimise(Domain<G, T>& domain, 
                  DataCollector<G, T>& data_collector) {

        _population = initialise_population();

        bool finished = false;
        unsigned gen = 1;

        do
        {
            //Evaluate population on domain
            domain.evaluate_population(_population, _num_trials, false);

            //Check for completion
            finished = optimisation_finished(gen, domain);

            //Collect data
            data_collector.collect_generational_data(_population, gen, finished);

            if(finished)
                break;

            //Optimiser step - creates next generation population
            _population = step();

            gen++;

        } while (!finished);

        _finished_gen = gen;

        if(domain.complete())
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

    auto clone() const
    {
        return std::unique_ptr<Optimiser>(clone_impl());
    }

    //Reset optimiser
    virtual void reset() = 0;

protected:

    //Applies evolutionary operators and creates new population
    //This is different for each optimiser
    virtual Population<G, T> step() = 0;

    //Each algorithm will intialise its population differently - implement that in here
    virtual Population<G, T> initialise_population() = 0;

    virtual Optimiser* clone_impl() const = 0;

    GPMap<G, T>& _gp_map;

    const unsigned _num_genes;
    const unsigned _max_gens;
    const unsigned _pop_size;
    const unsigned _num_trials;

    std::optional<unsigned> _finished_gen;

    Population<G, T> _population;

private: 

    bool optimisation_finished(const unsigned curr_gen, const Domain<G, T>& domain) const
    {
        if(curr_gen >= _max_gens ||
           domain.complete())
            return true;
        return false;
    }

};

} // namespace NeuroEvo

#endif
