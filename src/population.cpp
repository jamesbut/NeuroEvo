#include <population.h>
#include <iostream>

Population::Population(const unsigned int POP_SIZE, unsigned int& gen_ref,
                       PhenotypeSpec& PHENOTYPE_SPEC) :
    _POP_SIZE(POP_SIZE),
    _gen(gen_ref) {

    for(std::size_t i = 0; i < _POP_SIZE; i++)
        _organisms.push_back(Organism(PHENOTYPE_SPEC));

}

void Population::generate_new_population(Selection* selector,
                                         Mutation* genotype_mutator,
                                         Mutation* gp_map_mutator) {

    std::vector<Organism> new_pop;

    for(std::size_t i = 0; i < _POP_SIZE; i++) {

        //Select a genome using given selection operator
        Organism child_organism = selector->select(_organisms);

        //Mutate new genome
        if(genotype_mutator)
            genotype_mutator->mutate(child_organism.get_genotype().get_genes());

        //Mutate GPMap
        if(gp_map_mutator)
            gp_map_mutator->mutate(child_organism.get_gp_map()->get_map().get_vector());

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
