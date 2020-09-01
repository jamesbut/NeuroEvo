#include <phenotype/phenotype_specs/phenotype_spec.h>

namespace NeuroEvo {

PhenotypeSpec::PhenotypeSpec(const unsigned num_params, const bool trace) :
    _num_params(num_params),
    _trace(trace),
    _print_weights(false) {}

unsigned PhenotypeSpec::get_num_params() const
{ 
    return _num_params; 
}

bool PhenotypeSpec::get_trace() const 
{
    return _trace;
}

bool PhenotypeSpec::get_print_weights() const
{
    return _print_weights;
}

void PhenotypeSpec::set_trace(const bool trace)
{
    _trace = trace;
}

} // namespace NeuroEvo
