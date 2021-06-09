#include <util/maths/activation_functions/elu.h>
#include <cmath>

namespace NeuroEvo {

ELU::ELU(const double alpha) :
    ActivationFunction(NumberBound(-alpha), NumberBound::infinity()),
    _alpha(alpha) {}

double ELU::activate(const double x)
{
    return (x > 0) ? x : _alpha * (exp(x) - 1);
}

ELU* ELU::clone_impl() const
{
    return new ELU(*this);
}

} // namespace NeuroEvo
