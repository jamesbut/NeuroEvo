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

    Population(const unsigned pop_size, unsigned& gen_ref,
               GenotypeSpec<G>& geno_spec, PhenotypeSpec<G, T>& pheno_spec,
               GPMap<G, T>& gp_map) :
        _pop_size(pop_size),
        _gen(gen_ref) 
    {
        for(std::size_t i = 0; i < _pop_size; i++)
            _organisms.push_back(Organism(geno_spec, pheno_spec, gp_map));
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
        return _pop_size;
    }

    const unsigned& get_gen_num() const 
    {
        return _gen;
    }

    void set_organism_fitness(const std::size_t org, const double fitness) 
    {
        _organisms.at(org).set_fitness(fitness);
    }

    void organism_genesis(const std::size_t org)
    {
        _organisms.at(org).genesis();
    }

    //Generates new population with the provided genetic operators
    void generate_new_population(Selection<G, T>* selector,
                                 Mutator<G>* gp_map_mutator = nullptr) 
    {

        std::vector<Organism<G, T>> new_pop;

        for(std::size_t i = 0; i < _pop_size; i++) {

            //Select a genome using given selection operator
            Organism<G, T> child_organism = _organisms[i];
            if(selector)
                child_organism = selector->select(_organisms);

            //Mutate new genome
            child_organism.get_genotype().mutate();

            //Only mutate GPMap if a GPMap mutator has been specified
            //and if the GPMap is vectorisable
            //TODO: Removing GPMap mutation for now - might bring back later
            //if(gp_map_mutator && child_organism.get_gp_map()->get_map().has_value())
            //    gp_map_mutator->mutate(child_organism.get_gp_map()->get_map()->get_vector());

            //Create new phenotype out of newly modified genotype
            child_organism.genesis();

            //Add to new population
            new_pop.push_back(child_organism);

        }

        _organisms = new_pop;

    }

    friend std::ostream& operator<<(std::ostream& os, const Population& population)
    {
        for(std::size_t i = 0; i < population._organisms.size(); i++)
            os << "Org: " << i << " " << population._organisms[i] << std::endl;
        return os;
    }

private:

    const unsigned _pop_size;

    std::vector<Organism<G, T>> _organisms;

    unsigned& _gen;

};

} // namespace NeuroEvo

#endif
