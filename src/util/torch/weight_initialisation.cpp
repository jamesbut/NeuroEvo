#include <util/torch/weight_initialisation.h>
#include <torch/nn/modules/linear.h>
#include <torch/nn/init.h>

void set_module_weights_zeros(torch::nn::Module& module)
{
    torch::NoGradGuard no_grad;
    for(auto& module : module.modules())
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::zeros_(linear->weight);
            torch::nn::init::zeros_(linear->bias);
        }
}

void set_module_weights_ones(torch::nn::Module& module)
{
    torch::NoGradGuard no_grad;
    for(auto& module : module.modules())
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::ones_(linear->weight);
            torch::nn::init::ones_(linear->bias);
        }
}

void set_module_weights_gaussian(torch::nn::Module& module)
{
    torch::NoGradGuard no_grad;
    for(auto& module : module.modules())
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::normal_(linear->weight);
            torch::nn::init::normal_(linear->bias);
        }

}

