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
void set_module_weights_gaussian(torch::nn::Module& module,
                                 const double mean = 0., const double std_dev = 1.);

//Set all weights according to uniform distribution
void set_module_weights_uniform(torch::nn::Module& module,
                                const double lb = 0., const double ub = 1.);

void set_module_weights_kaiming_uniform(torch::nn::Module& module);
void set_module_weights_kaiming_normal(torch::nn::Module& module);

void set_module_weights_xavier_uniform(torch::nn::Module& module);
void set_module_weights_xavier_normal(torch::nn::Module& module);


#endif
