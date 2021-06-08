#include <util/maths/activation_functions/relu.h>

namespace NeuroEvo {

ReLU::ReLU() :
    ActivationFunction(NumberBound(0.), NumberBound::infinity()) {}

double ReLU::activate(const double x)
{
    return (x > 0) ? x : 0;
}

ReLU* ReLU::clone_impl() const
{
    return new ReLU(*this);
};

} // namespace NeuroEvo
