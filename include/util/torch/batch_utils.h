#ifndef _BATCH_UTILS_H_
#define _BATCH_UTILS_H_

#include <vector>
#include <torch/types.h>

namespace NeuroEvo {

//Generates batches from data and its respective labels
const std::vector<std::pair<torch::Tensor, torch::Tensor>>
    generate_batches(const unsigned batch_size,
                     const torch::Tensor& data,
                     const torch::Tensor& targets,
                     const bool shuffle_data = true);

} // namespace NeuroEvo

#endif
