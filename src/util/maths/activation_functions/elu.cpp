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

JSON ELU::to_json() const
{
    JSON json;
    json.emplace("name", "ELU");
    json.emplace("alpha", _alpha);
    return json;
}

ELU* ELU::clone_impl() const
{
    return new ELU(*this);
}

} // namespace NeuroEvo
