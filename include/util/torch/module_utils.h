#ifndef _MODULE_UTILS_H_
#define _MODULE_UTILS_H_

#include <torch/nn/module.h>
#include <torch/nn/modules/linear.h>

namespace NeuroEvo {

//Returns shared pointers to just the linear layers in a module
//Even though a vector of shared ptrs to Modules is returned, one can be sure that
//all those modules are linear modules
std::vector<std::shared_ptr<torch::nn::Module>>
    get_linear_layers(torch::nn::Module& module);

//Counts the number of weights (not biases) in the linear layers of a module
unsigned count_num_weights_linear(torch::nn::Module& module);

} // namespace NeuroEvo

#endif
