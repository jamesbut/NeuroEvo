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
#include <genetic_operators/mutation/mutation.h>
#include <phenotype/phenotype_specs/phenotype_spec.h>

namespace NeuroEvo {

class Population {

public:

    Population(const unsigned int POP_SIZE, unsigned int& gen_ref,
               Phenotypes::PhenotypeSpec& PHENOTYPE_SPEC);

    std::vector<Organism>& get_organisms();
    const unsigned int get_size() const;
    const unsigned int& get_gen_num() const;

    //Generates new population with the provided genetic operators
    void generate_new_population(Selectors::Selection* selector,
                                 Mutators::Mutation* genotype_mutator,
                                 Mutators::Mutation* gp_map_mutator);

private:

    const unsigned _POP_SIZE;

    std::vector<Organism> _organisms;

    unsigned& _gen;

};

} // namespace NeuroEvo

#endif
