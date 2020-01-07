#ifndef _PHENOTYPE_SPEC_H_
#define _PHENOTYPE_SPEC_H_

/*
    A PhenotypeSpec defines the specification of the phenotype
    to be used.
*/

#include <phenotype/phenotype.h>
#include <gp_map/gp_map.h>

namespace NeuroEvo {

namespace GPMaps {
    class GPMap;
} // namespace GPMaps

namespace Phenotypes {

//Generic phenotype spec
struct PhenotypeSpec {

public:

    virtual ~PhenotypeSpec() = default;

    PhenotypeSpec(const unsigned NUM_PARAMS) :
        NUM_PARAMS(NUM_PARAMS),
        PRINT_WEIGHTS(false),
        TRACE(false) {}

    //GPMap can be null
    virtual Phenotype* generate_phenotype(Genotypes::Genotype& genotype, GPMaps::GPMap* gp_map) = 0;

    auto clone() const { return std::unique_ptr<PhenotypeSpec>(clone_impl()); }

    const unsigned& get_num_params() { return NUM_PARAMS; }


    bool TRACE;
    bool PRINT_WEIGHTS;

protected:

    virtual PhenotypeSpec* clone_impl() const = 0;

    const unsigned NUM_PARAMS;

};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
