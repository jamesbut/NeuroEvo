#ifndef _PHENOTYPE_SPEC_H_
#define _PHENOTYPE_SPEC_H_

/*
    A PhenotypeSpec defines the specification of the phenotype
    to be used.
*/

#include <phenotype/phenotype.h>

namespace NeuroEvo {

template <typename G, typename T>
class PhenotypeSpec 
{

public:

    virtual ~PhenotypeSpec() = default;

    PhenotypeSpec(const unsigned num_params) :
        _num_params(num_params),
        _trace(false),
        _print_weights(false) {}

    auto clone() const 
    { 
        return std::unique_ptr<PhenotypeSpec>(clone_impl()); 
    }

    const unsigned& get_num_params() const
    { 
        return _num_params; 
    }

    bool get_trace() const 
    {
        return _trace;
    }

    bool get_print_weights() const
    {
        return _print_weights;
    }

protected:

    virtual PhenotypeSpec* clone_impl() const = 0;

    unsigned _num_params;

    bool _trace;
    bool _print_weights;

};

} // namespace NeuroEvo

#endif
