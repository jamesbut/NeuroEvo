#ifndef _RELU_SPEC_
#define _RELU_SPEC_

#include <torch/nn/modules/activation.h>
#include <util/maths/activation_functions/activation_function_specs/activation_function_spec.h>

namespace NeuroEvo {

class ReLUSpec : public ActivationFunctionSpec
{

public:

    torch::nn::AnyModule create_torch_module() const override
    {
        torch::nn::AnyModule relu_module((torch::nn::ReLU()));
        return relu_module;
    }

};

} // namespace NeuroEvo

#endif
