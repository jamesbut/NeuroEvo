#ifndef _REAL_VECTOR_PHENOTYPE_SPEC_H_
#define _REAL_VECTOR_PHENOTYPE_SPEC_H_

#include <phenotype/phenotype_specs/phenotype_spec.h>

namespace NeuroEvo {
namespace Phenotypes {

class RealVectorPhenotypeSpec : public PhenotypeSpec {

public:

    RealVectorPhenotypeSpec(const unsigned NUM_GENES);

    Genotypes::Genotype* generate_genotype() override;
    Genotypes::Genotype* generate_genotype(Genotypes::Genotype& genotype) override;
    Genotypes::Genotype* generate_genotype(const std::string& file_name) override;
    GPMaps::GPMap* generate_gp_map() override;
    GPMaps::GPMap* generate_gp_map(const std::string& file_name) override;
    Phenotypes::Phenotype* generate_phenotype(Genotypes::Genotype& genotype,
                                              GPMaps::GPMap* gp_map) override;

protected:

    virtual RealVectorPhenotypeSpec* clone_impl() const override {
        return new RealVectorPhenotypeSpec(*this);
    };

};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
