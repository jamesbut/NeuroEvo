#ifndef _IDENTICAL_TRAIT_MAP_
#define _IDENTICAL_TRAIT_MAP_

/*
 * This map takes the first value of the genome and uses it as the value for all of 
 * the traits of the phenotype vector.
 */

#include <gp_map/gp_map.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

template <typename G>
class IdenticalTraitMap : public GPMap<G, G>
{

public:

    IdenticalTraitMap(const VectorPhenotypeSpec* pheno_spec) :
        GPMap<G, G>(pheno_spec) {}

    Phenotype<G>* map(Genotype<G>& genotype) override {

       const G first_gene = genotype.genes().at(0); 

       std::vector<G> trait_vector(this->_pheno_spec->get_num_params(), first_gene); 

       return new VectorPhenotype<G>(trait_vector);

    } 

    void print(std::ostream& os) const override {}

private:

    IdenticalTraitMap<G>* clone_impl() const override
    {
        return new IdenticalTraitMap<G>(*this);
    }

};

} // namespace NeuroEvo

#endif

