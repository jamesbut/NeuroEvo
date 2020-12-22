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

    LeakyReLUSpec(const double negative_slope = 0.01) :
        ActivationFunctionSpec(),
        _negative_slope(negative_slope) {}

#if USE_TORCH
    torch::nn::AnyModule create_torch_module() const override
    {
        torch::nn::AnyModule leaky_relu_module(
            (torch::nn::LeakyReLU(torch::nn::LeakyReLUOptions().negative_slope(_negative_slope)))
        );
        return leaky_relu_module;
    }
#endif

    ActivationFunction* create_activation_function() const override
    {
        return new LeakyReLU(_negative_slope);
    }

private:

    const double _negative_slope;

    LeakyReLUSpec* clone_impl() const override
    {
        return new LeakyReLUSpec(*this);
    }

};

} // namespace NeuroEvo

#endif
