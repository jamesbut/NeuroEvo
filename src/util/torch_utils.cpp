#include <util/torch_utils.h>
#include <genotype/genotype_spec.h>

namespace NeuroEvo {

TorchNetwork* build_torch_network(
    NetworkBuilder& net_builder, 
    std::unique_ptr<Distribution<double>> init_net_weight_distr)
{
    
    if(!net_builder.is_torch_net())
    {
        std::cerr << "Cannot build a torch network from a builder that does not " << 
            "define a torch network!" << std::endl;
        exit(0);
    }

    //Create initial genotype 
    std::unique_ptr<Genotype<double>> init_genotype(new Genotype<double>());
    if(init_net_weight_distr)
    {
        GenotypeSpec<double> genotype_spec(net_builder.get_num_params(), 
                                           *init_net_weight_distr);
        init_genotype.reset(genotype_spec.generate_genotype());
    } 
        

    //Build and cast torch network
    Phenotype<double>* torch_net = net_builder.generate_phenotype(*init_genotype, nullptr);
    TorchNetwork* torch_net_cast = dynamic_cast<TorchNetwork*>(torch_net); 
    if(!torch_net_cast)
    {
        std::cerr << "Only TorchNetworks can be used to build a TorchNetwork!" << std::endl;
        exit(0);
    }


    return torch_net_cast; 

}

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
        std::shuffle(data_indexes.begin(), data_indexes.end(), std::default_random_engine{});

    int64_t current_data_index = 0;

    while(current_data_index < data.size(0))
    {

        //Build single batch
        auto remaining_data_size = data.size(0) - current_data_index;
        const int64_t this_batch_size = 
            remaining_data_size > (int)batch_size ? batch_size : remaining_data_size;

        //Allocate tensor space
        torch::Tensor data_batch = torch::zeros({this_batch_size, data.size(1)});
        torch::Tensor target_batch = torch::zeros({this_batch_size, targets.size(1)});

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
