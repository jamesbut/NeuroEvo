#include <population.h>
#include <iostream>

namespace NeuroEvo {

Population::Population(const unsigned int POP_SIZE, unsigned int& gen_ref,
                       Phenotypes::PhenotypeSpec& PHENOTYPE_SPEC) :
    _POP_SIZE(POP_SIZE),
    _gen(gen_ref) {

    for(std::size_t i = 0; i < _POP_SIZE; i++)
        _organisms.push_back(Organism(PHENOTYPE_SPEC));

}

void Population::generate_new_population(Selectors::Selection* selector,
                                         Mutators::Mutation* genotype_mutator,
                                         Mutators::Mutation* gp_map_mutator) {

    std::vector<Organism> new_pop;

    for(std::size_t i = 0; i < _POP_SIZE; i++) {

        //Select a genome using given selection operator
        Organism child_organism = selector->select(_organisms);

        //Mutate new genome
        if(genotype_mutator)
            genotype_mutator->mutate(child_organism.get_genotype().get_genes());

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

std::vector<Organism>& Population::get_organisms() {

    return _organisms;

}

const unsigned int Population::get_size() const {

    return _POP_SIZE;

}

const unsigned int& Population::get_gen_num() const {

    return _gen;

}

} // namespace NeuroEvo
