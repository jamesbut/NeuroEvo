#ifndef _SIGMOID_SPEC_
#define _SIGMOID_SPEC_

#if USE_TORCH
#include <torch/nn/modules/activation.h>
#endif
#include <util/maths/activation_functions/activation_function_specs/activation_function_spec.h>
#include <util/maths/activation_functions/sigmoid.h>
#include <data/json.h>

namespace NeuroEvo {

class SigmoidSpec : public ActivationFunctionSpec
{

public:

    SigmoidSpec();
    SigmoidSpec(const JSON& json);

#if USE_TORCH
    torch::nn::AnyModule create_torch_module() const override;
#endif
    ActivationFunction* create_activation_function() const override;

private:

    JSON to_json() const override;
    SigmoidSpec* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
