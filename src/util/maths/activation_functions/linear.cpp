#include <util/maths/activation_functions/linear.h>

namespace NeuroEvo {

Linear::Linear() :
    ActivationFunction(NumberBound::negative_inifinity(), NumberBound::infinity()) {}

double Linear::activate(const double x)
{
    return x;
}

Linear* Linear::clone_impl() const
{
    return new Linear(*this);
};

} // namespace NeuroEvo
