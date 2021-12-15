#include <util/maths/activation_functions/linear.h>

namespace NeuroEvo {

Linear::Linear() :
    ActivationFunction(NumberBound::negative_inifinity(), NumberBound::infinity()) {}

double Linear::activate(const double x)
{
    return x;
}

JSON Linear::to_json_impl() const
{
    JSON json;
    json.emplace("name", "Linear");
    return json;
}

Linear* Linear::clone_impl() const
{
    return new Linear(*this);
};

} // namespace NeuroEvo
