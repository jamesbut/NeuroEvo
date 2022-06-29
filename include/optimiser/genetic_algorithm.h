#ifndef _GENETIC_ALGORITHM_
#define _GENETIC_ALGORITHM_

#include <optimiser/optimiser.h>
#include <genetic_operators/selection/selection.h>

namespace NeuroEvo {

template <typename G, typename T>
class GeneticAlgorithm : public Optimiser<G, T>
{

public:

    GeneticAlgorithm(const unsigned num_genes,
                     const unsigned max_gens,
                     const unsigned pop_size,
                     std::shared_ptr<Selection<G, T>> selector,
                     std::shared_ptr<Mutator<G>> mutator,
                     std::shared_ptr<Distribution<G>> init_distr,
                     const bool quit_when_domain_complete = true,
                     const unsigned num_trials = 1,
                     const std::optional<unsigned>& seed = std::nullopt) :
        Optimiser<G, T>(num_genes, max_gens, pop_size,
                        quit_when_domain_complete, num_trials, seed),
        _selector(selector),
        _mutator(mutator),
        _init_distr(init_distr) {}

    GeneticAlgorithm(const GeneticAlgorithm& genetic_algorithm) :
        Optimiser<G, T>(genetic_algorithm),
        _selector(genetic_algorithm._selector),
        _mutator(genetic_algorithm._mutator),
        _init_distr(genetic_algorithm._init_distr) {}

    GeneticAlgorithm(const JSON& json) :
        GeneticAlgorithm(
            json.at({"num_genes"}),
            json.at({"num_gens"}),
            json.at({"pop_size"}),
            Factory<Selection<G, T>>::create(json.at({"Selection"})),
            Factory<Mutator<G>>::create(json.at({"Mutation"})),
            Factory<Distribution<G>>::create(json.at({"InitDistribution"})),
            json.at({"quit_domain_when_complete"}),
            json.at({"num_trials"})
        ) {}


    Population<G, T> step(std::shared_ptr<GPMap<G, T>> gp_map) override
    {

        std::vector<Organism<G, T>> new_orgs;
        new_orgs.reserve(this->_pop_size);

        for(unsigned i = 0; i < this->_pop_size; i++)
        {
            //Selection
            Organism<G, T> child_org = _selector->select(
                this->_population.get_organisms());

            //Mutation
            _mutator->mutate(child_org.get_genotype_mut().genes_mut());

            new_orgs.push_back(child_org);
        }

        return Population<G, T>(new_orgs);

    }

private:

    //Initialise population according to init_distr
    Population<G, T> initialise_population(std::shared_ptr<GPMap<G, T>> gp_map) override
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

        return Population<G, T>(genotypes, gp_map);

    }

    GeneticAlgorithm* clone_impl() const override
    {
        return new GeneticAlgorithm(*this);
    }

    void reset() override
    {

        _selector->reset(this->_seed);
        _mutator->reset(this->_seed);

        if(this->_seed.has_value())
            _init_distr->set_seed(this->_seed.value());
        else
            _init_distr->randomly_seed();

    }

    std::shared_ptr<Selection<G, T>> _selector;
    std::shared_ptr<Mutator<G>> _mutator;
    //Add crossover at some point

    std::shared_ptr<Distribution<G>> _init_distr;

};

static Factory<Optimiser<double, double>>::Registrar ga_registrar("GeneticAlgorithm",
    [](const JSON& json)
    {return std::make_shared<GeneticAlgorithm<double, double>>(json);});

} // namespace NeuroEvo

#endif
