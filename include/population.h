#ifndef _POPULATION_H_
#define _POPULATION_H_

/*
    This class builds a population of organisms.
    It does so by taking a PhenotypeSpec (description
    of the phenotype), population size and a reference
    to the current generation.
*/

#include <organism.h>
#include <genetic_operators/selection/selection.h>
#include <genetic_operators/mutation/mutator.h>
#include <phenotype/phenotype_specs/phenotype_spec.h>

namespace NeuroEvo {

template <typename G, typename T>
class Population {

public:

    Population() = default;

    Population(const std::vector<Genotype<G>>& genotypes,
               std::shared_ptr<GPMap<G, T>> gp_map)
    {
        for(const auto& genotype : genotypes)
            _organisms.push_back(Organism(genotype, gp_map));
    }

    Population(const std::vector<Organism<G, T>>& organisms) :
        _organisms(organisms)
    {
        genesis();
    }

    void genesis()
    {
        for(auto& org : _organisms)
            org.genesis();
    }

    const std::vector<Organism<G, T>>& get_organisms() const
    {
        return _organisms;
    }

    Organism<G, T>& get_mutable_organism(const std::size_t org)
    {
        return _organisms.at(org);
    }

    unsigned get_size() const
    {
        return _organisms.size();
    }

    const std::vector<double> get_fitnesses() const
    {
        std::vector<double> fitnesses(_organisms.size());
        for(std::size_t i = 0; i < _organisms.size(); i++)
            fitnesses[i] = _organisms[i].get_fitness().value();
        return fitnesses;
    }

    void set_organism_fitness(const std::size_t org, const double fitness,
                              const double domain_completion_fitness)
    {
        _organisms.at(org).set_fitness(fitness, domain_completion_fitness);
    }

    void organism_genesis(const std::size_t org)
    {
        _organisms.at(org).genesis();
    }

    const Organism<G, T>& get_fittest_org() const
    {
        double best_fitness = _organisms[0].get_fitness().value();
        std::size_t best_org_index = 0;
        for(unsigned i = 1; i < _organisms.size(); i++)
            if(_organisms[i].get_fitness().value() > best_fitness)
            {
                best_fitness = _organisms[i].get_fitness().value();
                best_org_index = i;
            }
        return _organisms[best_org_index];
    }

    friend std::ostream& operator<<(std::ostream& os, const Population& population)
    {
        for(std::size_t i = 0; i < population._organisms.size(); i++)
            os << "Org: " << i << " " << population._organisms[i] << std::endl;
        return os;
    }

private:

    std::vector<Organism<G, T>> _organisms;

};

} // namespace NeuroEvo

#endif
