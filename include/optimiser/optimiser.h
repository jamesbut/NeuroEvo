#ifndef _OPTIMISER_H_
#define _OPTIMISER_H_

/*
    Specifies an optimiser to create a new population in the evolutionary algorithm.
*/

#include <population.h>
#include <domains/domain.h>
#include <data/json.h>

namespace NeuroEvo {

template <typename G, typename T>
class Optimiser
{

public:

    //Optimiser(GPMap<G, T>& gp_map,
    Optimiser(const unsigned num_genes,
              const unsigned max_gens,
              const unsigned pop_size,
              const bool quit_when_domain_complete = true,
              const unsigned num_trials = 1,
              const std::optional<unsigned> seed = std::nullopt) :
        //_gp_map(gp_map),
        _num_genes(num_genes),
        _max_gens(max_gens),
        _pop_size(pop_size),
        _num_trials(num_trials),
        _seed(seed),
        _trace(false),
        _quit_when_domain_complete(quit_when_domain_complete) {}

    Optimiser(const Optimiser& optimiser) :
        //_gp_map(optimiser._gp_map),
        _num_genes(optimiser._num_genes),
        _max_gens(optimiser._max_gens),
        _pop_size(optimiser._pop_size),
        _num_trials(optimiser._num_trials),
        _seed(optimiser._seed),
        _trace(optimiser._trace),
        _quit_when_domain_complete(optimiser._quit_when_domain_complete) {}

    Optimiser(const JSON& json) :
        //_gp_map(nullptr),
        _num_genes(json.at("num_genes")),
        _max_gens(json.at("num_gens")),
        _pop_size(json.at("pop_size")),
        _num_trials(json.at("num_trials")),
        _seed(std::nullopt),
        _trace(false),
        _quit_when_domain_complete(json.at("quit_domain_when_complete")) {}

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
            data_collector.collect_generational_data(_population, gen, finished,
                                                     domain.get_hyperparams());

            if(finished) break;

            //Optimiser step - creates next generation population
            _population = step();

            gen++;

        } while (!finished);

        _finished_gen = gen;

        /*
        std::cout << "Best org:" << std::endl;
        const auto best_org = _population.get_fittest_org();
        std::cout << best_org << std::endl;
        */

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
    virtual Population<G, T> step() = 0;

    //Each algorithm will intialise its population differently - implement that in here
    virtual Population<G, T> initialise_population() = 0;

    virtual Optimiser* clone_impl() const = 0;

    //GPMap<G, T>& _gp_map;

    const unsigned _num_genes;
    const unsigned _max_gens;
    const unsigned _pop_size;
    const unsigned _num_trials;

    std::optional<unsigned> _finished_gen;

    Population<G, T> _population;

    std::optional<unsigned> _seed;

    bool _trace;

private:

    bool optimisation_finished(const unsigned curr_gen, const Domain<G, T>& domain) const
    {
        if(curr_gen >= _max_gens)
            return true;
        if(domain.complete() && _quit_when_domain_complete)
            return true;
        return false;
    }

    const bool _quit_when_domain_complete;

};

} // namespace NeuroEvo

#endif
