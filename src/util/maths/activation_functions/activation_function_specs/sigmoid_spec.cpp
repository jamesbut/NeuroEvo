#include <util/maths/activation_functions/activation_function_specs/sigmoid_spec.h>
#include <util/factory.h>

namespace NeuroEvo {

SigmoidSpec::SigmoidSpec() {}

SigmoidSpec::SigmoidSpec(const JSON& json) {}

#if USE_TORCH
torch::nn::AnyModule SigmoidSpec::create_torch_module() const
{
    torch::nn::AnyModule sigmoid_module((torch::nn::Sigmoid()));
    return sigmoid_module;
}
#endif

ActivationFunction* SigmoidSpec::create_activation_function() const
{
    return new Sigmoid();
}

JSON SigmoidSpec::to_json() const
{
    JSON json;
    json.emplace("name", "SigmoidSpec");
    return json;
}

SigmoidSpec* SigmoidSpec::clone_impl() const
{
    return new SigmoidSpec(*this);
}

static Factory<ActivationFunctionSpec>::Registrar sigmoid_spec_registrar("SigmoidSpec",
    [](const JSON& json) {return std::make_shared<SigmoidSpec>(json);});

} // namespace NeuroEvo
