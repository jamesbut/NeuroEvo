#ifndef _ELU_SPEC_H_
#define _ELU_SPEC_H_

#if USE_TORCH
#include <torch/nn/modules/activation.h>
#endif
#include <util/maths/activation_functions/activation_function_specs/activation_function_spec.h>
#include <util/maths/activation_functions/elu.h>

namespace NeuroEvo {

class ELUSpec : public ActivationFunctionSpec
{

public:

    ELUSpec(const double alpha = 1.) :
        _alpha(alpha) {}

#if USE_TORCH
    torch::nn::AnyModule create_torch_module() const override
    {
        torch::nn::AnyModule elu_module(
            (torch::nn::ELU(torch::nn::ELUOptions().alpha(_alpha)))
        );
        return elu_module;
    }
#endif

    ActivationFunction* create_activation_function() const override
    {
        return new ELU(_alpha);
    }

private:

    ELUSpec* clone_impl() const override
    {
        return new ELUSpec(*this);
    }

    const double _alpha;

};

} // namspace NeuroEvo

#endif
