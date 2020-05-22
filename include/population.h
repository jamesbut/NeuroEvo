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

template <typename G>
class Population {

public:

    Population(const unsigned pop_size, unsigned& gen_ref,
               GenotypeSpec<G>& geno_spec, PhenotypeSpec<G>& pheno_spec,
               GPMapSpec<G>* gp_map_spec) :
        _pop_size(pop_size),
        _gen(gen_ref) 
    {
        for(std::size_t i = 0; i < _pop_size; i++)
            _organisms.push_back(Organism(geno_spec, pheno_spec, gp_map_spec));
    }

    const std::vector<Organism<G>>& get_organisms() const
    {
        return _organisms;
    }

    Organism<G>& get_mutable_organism(const std::size_t org) 
    {
        return _organisms.at(org);
    }

    const unsigned get_size() const
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
    void generate_new_population(Selection<G>* selector,
                                 Mutator<G>* genotype_mutator,
                                 Mutator<G>* gp_map_mutator) 
    {

        std::vector<Organism<G>> new_pop;

        for(std::size_t i = 0; i < _pop_size; i++) {

            //Select a genome using given selection operator
            Organism child_organism = selector->select(_organisms);

            //Mutate new genome
            if(genotype_mutator)
                child_organism.get_genotype().mutate();

            //Only mutate GPMap if a GPMap mutator has been specified
            //and if the GPMap is vectorisable
            if(gp_map_mutator && child_organism.get_gp_map()->get_map().has_value())
                gp_map_mutator->mutate(child_organism.get_gp_map()->get_map()->get_vector());

            //Create new phenotype out of newly modified genotype
            child_organism.genesis();

            //Add to new population
            new_pop.push_back(child_organism);

        }

        _organisms = new_pop;

    }

private:

    const unsigned _pop_size;

    std::vector<Organism<G>> _organisms;

    unsigned& _gen;

};

} // namespace NeuroEvo

#endif
