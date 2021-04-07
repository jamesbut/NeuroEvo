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
    for(auto& module : module.modules(false))
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::ones_(linear->weight);
            torch::nn::init::ones_(linear->bias);
        }
}

void set_module_weights_gaussian(torch::nn::Module& module,
                                 const double mean, const double std_dev)
{
    torch::NoGradGuard no_grad;
    for(auto& module : module.modules())
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::normal_(linear->weight, mean, std_dev);
            torch::nn::init::normal_(linear->bias, mean, std_dev);
        }
}

void set_module_weights_uniform(torch::nn::Module& module,
                                const double lb, const double ub)
{
    torch::NoGradGuard no_grad;
    for(auto& module : module.modules())
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::uniform_(linear->weight, lb, ub);
            torch::nn::init::uniform_(linear->bias, lb, ub);
            //torch::nn::init::zeros_(linear->bias);
        }
}

void set_module_weights_kaiming_uniform(torch::nn::Module& module)
{
    torch::NoGradGuard no_grad;
    for(auto& module : module.modules(false))
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::kaiming_uniform_(linear->weight, std::sqrt(5));
            if(linear->bias.defined())
            {
                int64_t fan_in, fan_out;
                std::tie(fan_in, fan_out) =
                  torch::nn::init::_calculate_fan_in_and_fan_out(linear->weight);
                const auto bound = 1 / std::sqrt(fan_in);
                torch::nn::init::uniform_(linear->bias, -bound, bound);
                //torch::nn::init::zeros_(linear->bias);
            }
        }
}

void set_module_weights_kaiming_normal(torch::nn::Module& module)
{
    torch::NoGradGuard no_grad;
    for(auto& module : module.modules())
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::kaiming_normal_(linear->weight, std::sqrt(5));
            //This bias initialisation is just copied from kaiming uniform
            if(linear->bias.defined())
            {
                int64_t fan_in, fan_out;
                std::tie(fan_in, fan_out) =
                  torch::nn::init::_calculate_fan_in_and_fan_out(linear->weight);
                const auto bound = 1 / std::sqrt(fan_in);
                torch::nn::init::uniform_(linear->bias, -bound, bound);
                //torch::nn::init::zeros_(linear->bias);
            }
        }
}

void set_module_weights_xavier_uniform(torch::nn::Module& module)
{
    torch::NoGradGuard no_grad;
    for(auto& module : module.modules())
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::xavier_uniform_(linear->weight);
            //This bias initialisation is just copied from kaiming uniform
            if(linear->bias.defined())
            {
                int64_t fan_in, fan_out;
                std::tie(fan_in, fan_out) =
                  torch::nn::init::_calculate_fan_in_and_fan_out(linear->weight);
                const auto bound = 1 / std::sqrt(fan_in);
                torch::nn::init::uniform_(linear->bias, -bound, bound);
                //torch::nn::init::zeros_(linear->bias);
            }
        }
}

void set_module_weights_xavier_normal(torch::nn::Module& module)
{
    torch::NoGradGuard no_grad;
    for(auto& module : module.modules())
        if(auto* linear = module->as<torch::nn::Linear>())
        {
            torch::nn::init::xavier_normal_(linear->weight);
            //This bias initialisation is just copied from kaiming uniform
            if(linear->bias.defined())
            {
                int64_t fan_in, fan_out;
                std::tie(fan_in, fan_out) =
                  torch::nn::init::_calculate_fan_in_and_fan_out(linear->weight);
                const auto bound = 1 / std::sqrt(fan_in);
                torch::nn::init::uniform_(linear->bias, -bound, bound);
                //torch::nn::init::zeros_(linear->bias);
            }
        }
}
