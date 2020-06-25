#ifndef _TORCH_UTILS_H_
#define _TORCH_UTILS_H_

#include <phenotype/neural_network/torch_network.h>
#include <phenotype/phenotype_specs/network_builder.h>

namespace NeuroEvo {

//Build torch network depending on whether an initial weight distribution has been defined
TorchNetwork* build_torch_network(
    NetworkBuilder& net_builder, 
    std::unique_ptr<Distribution<double>> init_net_weight_distr =
        std::unique_ptr<Distribution<double>>(nullptr));

//Generates batches from data and its respective labels
const std::vector<std::pair<torch::Tensor, torch::Tensor>> 
    generate_batches(const unsigned batch_size, 
                     const torch::Tensor& data, 
                     const torch::Tensor& targets,
                     const bool shuffle_data = true);

} // namespace NeuroEvo

#endif
