#include <util/torch/module_utils.h>

namespace NeuroEvo {

std::vector<std::shared_ptr<torch::nn::Module>>
    get_linear_layers(torch::nn::Module& module)
{
    std::vector<std::shared_ptr<torch::nn::Module>> linear_ptrs;

    for(auto& module : module.modules(false))
        if(auto* linear = module->as<torch::nn::Linear>())
            linear_ptrs.push_back(module->shared_from_this());

    return linear_ptrs;
}


unsigned count_num_weights_linear(torch::nn::Module& module)
{
    unsigned num_weights = 0;

    for(auto& module : module.modules(false))
        if(auto* linear = module->as<torch::nn::Linear>())
            num_weights += torch::numel(linear->weight);

    return num_weights;
}

} // namespace NeuroEvo
