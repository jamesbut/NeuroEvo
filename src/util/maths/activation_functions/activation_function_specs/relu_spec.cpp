#include <util/maths/activation_functions/activation_function_specs/relu_spec.h>
#include <util/factory.h>

namespace NeuroEvo {

ReLUSpec::ReLUSpec() {}

ReLUSpec::ReLUSpec(const JSON& json) {}

#if USE_TORCH
torch::nn::AnyModule ReLUSpec::create_torch_module() const
{
    torch::nn::AnyModule relu_module((torch::nn::ReLU()));
    return relu_module;
}
#endif

ActivationFunction* ReLUSpec::create_activation_function() const
{
    return new ReLU();
}

JSON ReLUSpec::to_json() const
{
    JSON json;
    json.emplace("name", "ReLUSpec");
    return json;
}

ReLUSpec* ReLUSpec::clone_impl() const
{
    return new ReLUSpec(*this);
}

static Factory<ActivationFunctionSpec>::Registrar relu_spec_registrar("ReLUSpec",
    [](const JSON& json) {return std::make_shared<ReLUSpec>(json);});

} // namespace NeuroEvo
