#ifndef _REAL_VECTOR_PHENOTYPE_SPEC_H_
#define _REAL_VECTOR_PHENOTYPE_SPEC_H_

#include <phenotype/phenotype_specs/phenotype_spec.h>

namespace NeuroEvo {

//A RealVectorPhenotype can only be generated from a genes of type double for now
//This can easily be made generic in the future
class RealVectorPhenotypeSpec : public PhenotypeSpec<double> {

public:

    RealVectorPhenotypeSpec(const unsigned num_genes);

    Phenotype* generate_phenotype(Genotype<double>& genotype,
                                  GPMap<double>* gp_map) override;

protected:

    virtual RealVectorPhenotypeSpec* clone_impl() const override 
    {
        return new RealVectorPhenotypeSpec(*this);
    };

};

} // namespace NeuroEvo

#endif
