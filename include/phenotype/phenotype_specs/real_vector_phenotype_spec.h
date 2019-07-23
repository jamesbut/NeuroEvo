#ifndef _REAL_VECTOR_PHENOTYPE_SPEC_H_
#define _REAL_VECTOR_PHENOTYPE_SPEC_H_

#include <phenotype/phenotype_specs/phenotype_spec.h>

class RealVectorPhenotypeSpec : public PhenotypeSpec {

public:

    RealVectorPhenotypeSpec(const unsigned int NUM_GENES);

    Genotype* generate_genotype() override;
    Genotype* generate_genotype(Genotype& genotype) override;
    Genotype* generate_genotype(const std::string& file_name) override;
    GPMap* generate_gp_map() override;
    GPMap* generate_gp_map(const std::string& file_name) override;
    Phenotype* generate_phenotype(Genotype& genotype, GPMap* gp_map) override;

protected:

    virtual RealVectorPhenotypeSpec* clone_impl() const override {
        return new RealVectorPhenotypeSpec(*this);
    };

};

#endif
