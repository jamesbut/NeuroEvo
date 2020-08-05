#ifndef _SIGMOID_SPEC_
#define _SIGMOID_SPEC_

#if TORCH_FOUND
#include <torch/nn/modules/activation.h>
#endif
#include <util/maths/activation_functions/activation_function_specs/activation_function_spec.h>
#include <util/maths/activation_functions/sigmoid.h>

namespace NeuroEvo {

class SigmoidSpec : public ActivationFunctionSpec
{

public:

#if TORCH_FOUND
    torch::nn::AnyModule create_torch_module() const override
    {
        torch::nn::AnyModule sigmoid_module((torch::nn::Sigmoid()));
        return sigmoid_module;
    }
#endif

    ActivationFunction* create_activation_function() const override
    {
        return new Sigmoid(); 
    }

protected:

    SigmoidSpec* clone_impl() const override
    {
        return new SigmoidSpec(*this);
    }

};

} // namespace NeuroEvo

#endif
