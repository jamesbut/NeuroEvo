#include <util/torch/batch_utils.h>
#include <random>

namespace NeuroEvo {

const std::vector<std::pair<torch::Tensor, torch::Tensor>>
    generate_batches(const unsigned batch_size,
                     const torch::Tensor& data,
                     const torch::Tensor& targets,
                     const bool shuffle_data)
{

    std::vector<std::pair<torch::Tensor, torch::Tensor>> batches;

    //Shuffle data
    std::vector<int64_t> data_indexes(data.size(0));
    std::iota(data_indexes.begin(), data_indexes.end(), 0);
    if(shuffle_data)
        std::shuffle(data_indexes.begin(), data_indexes.end(),
                     std::default_random_engine{});

    int64_t current_data_index = 0;

    while(current_data_index < data.size(0))
    {

        //Build single batch
        auto remaining_data_size = data.size(0) - current_data_index;
        const int64_t this_batch_size =
            remaining_data_size > (int)batch_size ? batch_size : remaining_data_size;

        //Allocate tensor space
        torch::Tensor data_batch = torch::zeros({this_batch_size, data.size(1)},
                                                {torch::kFloat64});
        torch::Tensor target_batch = torch::zeros({this_batch_size, targets.size(1)},
                                                  {torch::kFloat64});

        //Move data into the tensors
        for(int64_t i = 0; i < this_batch_size; i++)
        {
            auto index = data_indexes[current_data_index];
            data_batch.index_put_({i}, data.index({index}));
            target_batch.index_put_({i}, targets.index({index}));
            current_data_index++;
        }

        //Push batch onto batches
        batches.push_back(std::pair<torch::Tensor, torch::Tensor>(data_batch,
                                                                  target_batch));
    }

    return batches;

}
} // namespace NeuroEvo
