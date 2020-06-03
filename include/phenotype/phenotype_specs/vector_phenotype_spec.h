#ifndef _VECTOR_PHENOTYPE_SPEC_H_
#define _VECTOR_PHENOTYPE_SPEC_H_

#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

//This class describes how to build a generic VectorPhenotype from a generic 
//genotype and gp map - right now the genes and traits must match type
template <typename G>
class VectorPhenotypeSpec : public PhenotypeSpec<G, G> {

public:

    VectorPhenotypeSpec(const unsigned num_genes) :
        PhenotypeSpec<G, G>(num_genes) {}

    Phenotype<G>* generate_phenotype(Genotype<G>& genotype,
                                     GPMap<G, G>* gp_map) override
    {
        if(gp_map)
            return gp_map->map(genotype, *this);
        else
            return new VectorPhenotype<G>(genotype.genes());
    }

protected:

    VectorPhenotypeSpec* clone_impl() const override 
    {
        return new VectorPhenotypeSpec(*this);
    };

};

} // namespace NeuroEvo

#endif
