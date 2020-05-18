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

    Population(const unsigned int pop_size, unsigned int& gen_ref,
               GenotypeSpec<G>& geno_spec, PhenotypeSpec<G>& pheno_spec,
               GPMapSpec<G>* gp_map_spec);

    std::vector<Organism<G>>& get_organisms();
    const unsigned int get_size() const;
    const unsigned int& get_gen_num() const;

    //Generates new population with the provided genetic operators
    void generate_new_population(Selection<G>* selector,
                                 Mutator<G>* genotype_mutator,
                                 Mutator<G>* gp_map_mutator);

private:

    const unsigned _pop_size;

    std::vector<Organism<G>> _organisms;

    unsigned& _gen;

};

} // namespace NeuroEvo

#endif
