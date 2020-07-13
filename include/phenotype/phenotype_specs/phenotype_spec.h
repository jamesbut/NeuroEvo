#ifndef _PHENOTYPE_SPEC_H_
#define _PHENOTYPE_SPEC_H_

/*
    A PhenotypeSpec defines the specification of the phenotype
    to be used.
*/

#include <phenotype/phenotype.h>

namespace NeuroEvo {

class PhenotypeSpec 
{

public:

    PhenotypeSpec(const unsigned num_params);

    virtual ~PhenotypeSpec() = default;

    const unsigned& get_num_params() const;
    bool get_trace() const; 
    bool get_print_weights() const;

    auto clone() const
    { 
        return std::unique_ptr<PhenotypeSpec>(clone_impl()); 
    }

protected:

    virtual PhenotypeSpec* clone_impl() const = 0;

    unsigned _num_params;

    bool _trace;
    bool _print_weights;

};

} // namespace NeuroEvo

#endif
