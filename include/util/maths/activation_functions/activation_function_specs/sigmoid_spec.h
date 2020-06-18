#ifndef _SIGMOID_SPEC_
#define _SIGMOID_SPEC_

#include <torch/nn/modules/activation.h>
#include <util/maths/activation_functions/activation_function_specs/activation_function_spec.h>

namespace NeuroEvo {

class SigmoidSpec : public ActivationFunctionSpec
{

public:

    torch::nn::AnyModule create_torch_module() const override
    {
        torch::nn::AnyModule sigmoid_module((torch::nn::Sigmoid()));
        return sigmoid_module;
    }

};

} // namespace NeuroEvo

#endif
