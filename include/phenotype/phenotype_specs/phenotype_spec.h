#ifndef _PHENOTYPE_SPEC_H_
#define _PHENOTYPE_SPEC_H_

/*
    A PhenotypeSpec defines the specification of the phenotype
    to be used.
    It also describes how to generate the phenotype from the genotype
    and GPMap.
*/

#include <phenotype/phenotype.h>
#include <gp_map/gp_map.h>

namespace NeuroEvo {

template <typename G>
class GPMap;

//Generic phenotype spec
template <typename G>
class PhenotypeSpec {

public:

    virtual ~PhenotypeSpec() = default;

    PhenotypeSpec(const unsigned num_params) :
        _num_params(num_params),
        _print_weights(false),
        _trace(false) {}

    //GPMap can be null
    virtual Phenotype* generate_phenotype(Genotype<G>& genotype, GPMap<G>* gp_map) = 0;

    auto clone() const { return std::unique_ptr<PhenotypeSpec>(clone_impl()); }

    const unsigned& get_num_params() { return _num_params; }

    bool _trace;
    bool _print_weights;

protected:

    virtual PhenotypeSpec* clone_impl() const = 0;

    const unsigned _num_params;

};

} // namespace NeuroEvo

#endif
