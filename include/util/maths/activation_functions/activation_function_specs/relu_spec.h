#ifndef _RELU_SPEC_H_
#define _RELU_SPEC_H_

#if USE_TORCH
#include <torch/nn/modules/activation.h>
#endif
#include <util/maths/activation_functions/activation_function_specs/activation_function_spec.h>
#include <util/maths/activation_functions/relu.h>
#include <data/json.h>

namespace NeuroEvo {

class ReLUSpec : public ActivationFunctionSpec
{

public:

    ReLUSpec();
    ReLUSpec(const JSON& json);

#if USE_TORCH
    torch::nn::AnyModule create_torch_module() const override;
#endif
    ActivationFunction* create_activation_function() const override;

private:

    ReLUSpec* clone_impl() const override;

};


} // namespace NeuroEvo

#endif
