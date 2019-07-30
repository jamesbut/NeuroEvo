#ifndef _PHENOTYPE_SPEC_H_
#define _PHENOTYPE_SPEC_H_

/*
    A PhenotypeSpec defines the specification of the phenotype
    to be used.
*/

#include <phenotype/phenotype.h>
#include <optional>
#include <gp_map/gp_map.h>

namespace NeuroEvo {

namespace GPMaps {
    class GPMap;
} // namespace GPMaps

namespace Phenotypes {

//Generic phenotype spec
struct PhenotypeSpec {

public:

    PhenotypeSpec(const unsigned int NUM_GENES, const bool TRACE = false) :
        NUM_GENES(NUM_GENES),
        PRINT_WEIGHTS(false),
        TRACE(TRACE) {}

    virtual Genotypes::Genotype* generate_genotype() = 0;

    //Generates a copy of the genotype
    //TODO: Remove this if not needed - I can just do genotype.clone()
    virtual Genotypes::Genotype* generate_genotype(Genotypes::Genotype& genotype) = 0;

    //Generates a genotype from file
    virtual Genotypes::Genotype* generate_genotype(const std::string& file_name) = 0;

    virtual GPMaps::GPMap* generate_gp_map() = 0;
    virtual GPMaps::GPMap* generate_gp_map(const std::string& file_name) = 0;

    //GPMap can be null
    virtual Phenotype* generate_phenotype(Genotypes::Genotype& genotype, GPMaps::GPMap* gp_map) = 0;

    auto clone() const { return std::unique_ptr<PhenotypeSpec>(clone_impl()); }

    const unsigned int NUM_GENES;

    bool TRACE;
    bool PRINT_WEIGHTS;

protected:

    virtual PhenotypeSpec* clone_impl() const = 0;

};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
