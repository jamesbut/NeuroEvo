#ifndef _ACTIVATION_FUNCTION_SPEC_
#define _ACTIVATION_FUNCTION_SPEC_

#include <torch/nn/modules/container/any.h>

/*
 * Abstrace interface for defining an activation function specification.
 * It requires functions that create the torch modules from the spec.
 */

namespace NeuroEvo {

class ActivationFunctionSpec 
{

public:

    //Define how to create torch module from activation function spec
    virtual torch::nn::AnyModule create_torch_module() const = 0;

};

} // namespace NeuroEvo

#endif
