#include <util/maths/activation_functions/leaky_relu.h>

namespace NeuroEvo {

LeakyReLU::LeakyReLU(const double negative_slope) :
    ActivationFunction(NumberBound::negative_inifinity(), NumberBound::infinity()),
    _negative_slope(negative_slope) {}

double LeakyReLU::activate(const double x)
{
    return (x > 0) ? x : _negative_slope * x;
}

JSON LeakyReLU::to_json() const
{
    JSON json;
    json.emplace("name", "LeakyReLU");
    json.emplace("negative_slope", _negative_slope);
    return json;
}

LeakyReLU* LeakyReLU::clone_impl() const
{
    return new LeakyReLU(*this);
}

} // namespace NeuroEvo
