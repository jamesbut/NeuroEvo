#include <util/maths/activation_functions/activation_function_specs/leaky_relu_spec.h>

namespace NeuroEvo {

LeakyReLUSpec::LeakyReLUSpec(const double negative_slope) :
    ActivationFunctionSpec(),
    _negative_slope(negative_slope) {}

#if USE_TORCH
torch::nn::AnyModule LeakyReLUSpec::create_torch_module() const
{
    torch::nn::AnyModule leaky_relu_module(
        (torch::nn::LeakyReLU(
             torch::nn::LeakyReLUOptions().negative_slope(_negative_slope)))
    );
    return leaky_relu_module;
}
#endif

ActivationFunction* LeakyReLUSpec::create_activation_function() const
{
    return new LeakyReLU(_negative_slope);
}

JSON LeakyReLUSpec::to_json() const
{
    JSON json;
    json.emplace("name", "LeakyReLUSpec");
    json.emplace("negative_slope", _negative_slope);
    return json;
}

LeakyReLUSpec* LeakyReLUSpec::clone_impl() const
{
    return new LeakyReLUSpec(*this);
}

} // namespace NeuroEvo
