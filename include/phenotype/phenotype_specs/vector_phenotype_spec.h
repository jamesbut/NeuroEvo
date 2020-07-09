#ifndef _VECTOR_PHENOTYPE_SPEC_H_
#define _VECTOR_PHENOTYPE_SPEC_H_

#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

template <typename G>
class VectorPhenotypeSpec : public PhenotypeSpec<G, G> {

public:

    VectorPhenotypeSpec(const unsigned num_genes) :
        PhenotypeSpec<G, G>(num_genes) {}

protected:

    VectorPhenotypeSpec* clone_impl() const override 
    {
        return new VectorPhenotypeSpec(*this);
    };

};

} // namespace NeuroEvo

#endif
