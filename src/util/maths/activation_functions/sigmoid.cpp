#include <util/maths/activation_functions/sigmoid.h>

namespace NeuroEvo {

Sigmoid::Sigmoid(const double k) :
    ActivationFunction(NumberBound(0.), NumberBound(1.)),
    _k(k) {}

double Sigmoid::activate(const double x)
{
    return 1 / (1 + exp(-x / _k));
}

JSON Sigmoid::to_json() const
{
    JSON json;
    json.emplace("name", "Sigmoid");
    return json;
}

Sigmoid* Sigmoid::clone_impl() const
{
    return new Sigmoid(*this);
};

} // namespace NeuroEvo
