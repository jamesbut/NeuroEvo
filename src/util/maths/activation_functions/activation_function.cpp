#include <util/maths/activation_functions/activation_function.h>

namespace NeuroEvo {

ActivationFunction::ActivationFunction(const NumberBound& lower_bound,
                                       const NumberBound& upper_bound) :
    _lower_bound(lower_bound),
    _upper_bound(upper_bound) {}

const NumberBound& ActivationFunction::get_lower_bound() const
{
    return _lower_bound;
}

const NumberBound& ActivationFunction::get_upper_bound() const
{
    return _upper_bound;
}

} // namespace NeuroEvo

