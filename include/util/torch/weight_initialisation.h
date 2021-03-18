#ifndef _WEIGHT_INITIALISATION_H_
#define _WEIGHT_INITIALISATION_H_

//A number of functions to set the weights and biases of linear torch modules.
//These functions are also applied to all submodules of module.

#include <torch/nn/module.h>

//Set all weights to zero
void set_module_weights_zeros(torch::nn::Module& module);

//Set all weights to one
void set_module_weights_ones(torch::nn::Module& module);

//Set all weights according to a gaussian distribution
void set_module_weights_gaussian(torch::nn::Module& module);


#endif
