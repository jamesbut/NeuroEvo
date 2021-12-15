#ifndef _LINEAR_SPEC_H_
#define _LINEAR_SPEC_H_

#if USE_TORCH
#include <torch/nn/modules/activation.h>
#endif
#include <util/maths/activation_functions/activation_function_specs/activation_function_spec.h>
#include <util/maths/activation_functions/linear.h>

namespace NeuroEvo {

class LinearSpec : public ActivationFunctionSpec
{

public:

#if USE_TORCH
    torch::nn::AnyModule create_torch_module() const override;
#endif
    ActivationFunction* create_activation_function() const override;

private:

    JSON to_json() const override;
    LinearSpec* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
