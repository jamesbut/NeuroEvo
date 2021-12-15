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

    ELUSpec(const double alpha = 1.);

#if USE_TORCH
    torch::nn::AnyModule create_torch_module() const override;
#endif
    ActivationFunction* create_activation_function() const override;

private:

    JSON to_json() const override;
    ELUSpec* clone_impl() const override;

    const double _alpha;

};

} // namspace NeuroEvo

#endif
