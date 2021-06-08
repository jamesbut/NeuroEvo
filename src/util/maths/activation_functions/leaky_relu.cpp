#include <util/maths/activation_functions/leaky_relu.h>

namespace NeuroEvo {

LeakyReLU::LeakyReLU(const double negative_slope) :
    ActivationFunction(NumberBound::negative_inifinity(), NumberBound::infinity()),
    _negative_slope(negative_slope) {}

double LeakyReLU::activate(const double x)
{
    return (x > 0) ? x : _negative_slope * x;
}

LeakyReLU* LeakyReLU::clone_impl() const
{
    return new LeakyReLU(*this);
}

} // namespace NeuroEvo
