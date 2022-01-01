#include <util/maths/activation_functions/activation_function_specs/linear_spec.h>
#include <util/factory.h>

namespace NeuroEvo {

LinearSpec::LinearSpec(const JSON& json) {}

#if USE_TORCH
torch::nn::AnyModule LinearSpec::create_torch_module() const
{
    torch::nn::AnyModule identity_module((torch::nn::Identity()));
    return identity_module;
}
#endif

ActivationFunction* LinearSpec::create_activation_function() const
{
    return new Linear();
}

JSON LinearSpec::to_json() const
{
    JSON json;
    json.emplace("name", "LinearSpec");
    return json;
}

LinearSpec* LinearSpec::clone_impl() const
{
    return new LinearSpec(*this);
}

static Factory<ActivationFunctionSpec>::Registrar linear_spec_registrar("LinearSpec",
    [](const JSON& json) {return std::make_shared<LinearSpec>(json);});

} // namespace NeuroEvo
