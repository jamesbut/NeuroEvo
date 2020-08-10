#ifndef _ACTIVATION_FUNCTION_SPEC_
#define _ACTIVATION_FUNCTION_SPEC_

#include <util/maths/activation_functions/activation_function.h>
#if USE_TORCH
#include <torch/nn/modules/container/any.h>
#endif

/*
 * Abstrace interface for defining an activation function specification.
 * It requires functions that create the torch modules from the spec.
 */

namespace NeuroEvo {

class ActivationFunctionSpec 
{

public:

    virtual ~ActivationFunctionSpec() = default;

#if USE_TORCH
    //Define how to create torch module from activation function spec
    virtual torch::nn::AnyModule create_torch_module() const = 0;
#endif

    //Define how to create a NeuroEvo activation function
    virtual ActivationFunction* create_activation_function() const = 0;

    auto clone() const 
    {
        return std::unique_ptr<ActivationFunctionSpec>(clone_impl());
    }

protected:

    virtual ActivationFunctionSpec* clone_impl() const = 0;

};

} // namespace NeuroEvo

#endif
