#ifndef _GENETIC_ALGORITHM_
#define _GENETIC_ALGORITHM_

#include <optimiser/optimiser.h>
#include <genetic_operators/selection/selection.h>

namespace NeuroEvo {

template <typename G, typename T>
class GeneticAlgorithm : public Optimiser<G, T>
{

public:

    GeneticAlgorithm(Selection<G, T>& selector,
                     Mutator<G>& mutator,
                     Distribution<G>& init_distr,
                     GPMap<G, T>& gp_map,
                     const unsigned num_genes,
                     const unsigned max_gens,
                     const unsigned pop_size,
                     const unsigned num_trials = 1) :
        Optimiser<G, T>(gp_map, num_genes, max_gens, pop_size, num_trials),
        _selector(selector.clone()),
        _mutator(mutator.clone()),
        _init_distr(init_distr.clone()) {}

    GeneticAlgorithm(const GeneticAlgorithm& genetic_algorithm) :
        Optimiser<G, T>(genetic_algorithm._gp_map, genetic_algorithm._num_genes,
                        genetic_algorithm._max_gens, genetic_algorithm._pop_size,
                        genetic_algorithm._num_trials),
        _selector(genetic_algorithm._selector->clone()),
        _mutator(genetic_algorithm._mutator->clone()),
        _init_distr(genetic_algorithm._init_distr->clone()) {}

    Population<G, T> step() override
    {

        std::vector<Organism<G, T>> new_orgs;
        new_orgs.reserve(this->_pop_size);

        for(unsigned i = 0; i < this->_pop_size; i++)
        {
            //Selection
            Organism<G, T> child_org = _selector->select(this->_population.get_organisms());

            //Mutation
            _mutator->mutate(child_org.get_genotype().genes());
            
            new_orgs.push_back(child_org);
        }

        return Population<G, T>(new_orgs);

    }

private:

    //Initialise population according to init_distr
    Population<G, T> initialise_population() override
    {

        std::vector<Genotype<G>> genotypes;
        genotypes.reserve(this->_pop_size);

        for(unsigned i = 0; i < this->_pop_size; i++)
        {
            std::vector<G> genes;
            genes.reserve(this->_num_genes);
            for(unsigned j = 0; j < this->_num_genes; j++)
                genes.push_back(_init_distr->next());
            genotypes.push_back(Genotype<G>(genes));
        }

        return Population<G, T>(genotypes, this->_gp_map);
        
    }

    GeneticAlgorithm* clone_impl() const override
    {
        return new GeneticAlgorithm(*this);
    }

    //Nothing to reset in the GA
    void reset() override {}

    std::unique_ptr<Selection<G, T>> _selector;
    std::unique_ptr<Mutator<G>> _mutator;
    //Add crossover at some point
    
    std::unique_ptr<Distribution<G>> _init_distr;

};

} // namespace NeuroEvo

#endif
