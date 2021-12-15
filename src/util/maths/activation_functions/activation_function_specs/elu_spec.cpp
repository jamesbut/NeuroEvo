#include <util/maths/activation_functions/activation_function_specs/elu_spec.h>

namespace NeuroEvo {

ELUSpec::ELUSpec(const double alpha) :
    _alpha(alpha) {}

#if USE_TORCH
torch::nn::AnyModule ELUSpec::create_torch_module() const
{
    torch::nn::AnyModule elu_module(
        (torch::nn::ELU(torch::nn::ELUOptions().alpha(_alpha)))
    );
    return elu_module;
}
#endif

ActivationFunction* ELUSpec::create_activation_function() const
{
    return new ELU(_alpha);
}

JSON ELUSpec::to_json() const
{
    JSON json;
    json.emplace("name", "ELUSpec");
    json.emplace("alpha", _alpha);
    return json;
}

ELUSpec* ELUSpec::clone_impl() const
{
    return new ELUSpec(*this);
}

} // namespace NeuroEvo
