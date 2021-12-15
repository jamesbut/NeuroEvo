#ifndef _LEAKY_RELU_SPEC_H_
#define _LEAKY_RELU_SPEC_H_

#if USE_TORCH
#include <torch/nn/modules/activation.h>
#endif
#include <util/maths/activation_functions/activation_function_specs/activation_function_spec.h>
#include <util/maths/activation_functions/leaky_relu.h>

namespace NeuroEvo {

class LeakyReLUSpec : public ActivationFunctionSpec
{

public:

    LeakyReLUSpec(const double negative_slope = 0.01);

#if USE_TORCH
    torch::nn::AnyModule create_torch_module() const override;
#endif
    ActivationFunction* create_activation_function() const override;

private:

    JSON to_json() const override;
    LeakyReLUSpec* clone_impl() const override;

    const double _negative_slope;

};

} // namespace NeuroEvo

#endif
