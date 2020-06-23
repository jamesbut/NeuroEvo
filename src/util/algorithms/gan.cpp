#include <ATen/Functions.h>
#include <ATen/core/TensorBody.h>
#include <torch/data/dataloader.h>
#include <torch/nn/options/loss.h>
#include <util/algorithms/gan.h>
#include <genotype/genotype_spec.h>
#include <util/random/gaussian_distribution.h>

namespace NeuroEvo {

GAN::GAN(torch::Tensor& real_data, NetworkBuilder& generator_builder,
         NetworkBuilder& discriminator_builder, 
         std::unique_ptr<Distribution<double>> init_net_weight_distr) :
    _real_data(real_data),
    _generator(build_network(generator_builder, init_net_weight_distr)),
    _discriminator(build_network(discriminator_builder, init_net_weight_distr)) {}

void GAN::train(const unsigned num_epochs, const unsigned batch_size)
{

    //Generate fake data
    const double fake_data_mean = 10.;
    const double fake_data_stddev = 1.;
    auto fake_data_distribution = GaussianDistribution(fake_data_mean, fake_data_stddev);

    torch::Tensor fake_data = torch::zeros({_real_data.size(0), _real_data.size(1)}); 
    for(unsigned i = 0; i < fake_data.size(0); i++)
        for(unsigned j = 0; j < fake_data.size(1); j++)
            fake_data[i][j] = fake_data_distribution.next();

    //This was part of the tutorial but stop loss reducing to zero - maybe it is important
    //for a GAN
    //torch::Tensor real_labels = torch::empty(_real_data.size(0)).uniform_(0.8, 1.0);
    torch::Tensor real_labels = torch::ones(_real_data.size(0));
    torch::Tensor fake_labels = torch::zeros(fake_data.size(0));

    const double discriminator_learning_rate = 5e-4;
    //const double discriminator_learning_rate = 1e-3;

    torch::optim::Adam discriminator_optimizer(
        _discriminator->parameters(), torch::optim::AdamOptions(discriminator_learning_rate)
    );
    
    for(unsigned i = 0; i < num_epochs; i++)
    {

        //Don't use generator for now, lets just make some random fake data
        //lets just train a discriminator
        
        //Concatenate data tensors
        torch::Tensor data = torch::cat({_real_data, fake_data});
        torch::Tensor data_labels = torch::cat({real_labels, fake_labels});
        
        //Generate batches
        const std::vector<std::pair<torch::Tensor, torch::Tensor>> batches =
            generate_batches(batch_size, data, data_labels);

        torch::Tensor total_d_loss = torch::zeros(1);

        for(const auto& batch : batches)
        {

            _discriminator->zero_grad();

            //Train on batch
            torch::Tensor d_output = _discriminator->forward(batch.first);

            //BCE loss with summed reduction (just sums the loss for the batch)
            torch::Tensor d_loss = torch::nn::functional::binary_cross_entropy(
                d_output, 
                batch.second,
                torch::nn::functional::BinaryCrossEntropyFuncOptions().reduction(torch::kSum)
            );

            total_d_loss += d_loss;
            d_loss.backward();

            discriminator_optimizer.step();

        }

        //Divide by total number of data points seen
        torch::Tensor avg_d_loss = total_d_loss / data.size(0);
        
        std::cout << "Epoch: " << i << " | Discriminator loss: " 
            << avg_d_loss.item<float>() << std::endl;
 
    }

}

torch::Tensor GAN::test_discriminator(const torch::Tensor& x) const
{
    return _discriminator->forward(x);
}

//Build network depending on whether an initial weight distribution has been defined
TorchNetwork* GAN::build_network(NetworkBuilder& net_builder, 
                                 std::unique_ptr<Distribution<double>>& init_net_weight_distr)
{
    
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
        std::cerr << "Only TorchNetworks can be used to build a GAN!" << std::endl;
        exit(0);
    }

    return torch_net_cast; 

}

const std::vector<std::pair<torch::Tensor, torch::Tensor>> 
    GAN::generate_batches(const unsigned batch_size, 
                          const torch::Tensor& data, 
                          const torch::Tensor& data_labels,
                          const bool shuffle_data) const
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
        torch::Tensor label_batch = torch::zeros({this_batch_size, 1});

        //Move data into the tensors
        for(int64_t i = 0; i < this_batch_size; i++)
        {
            auto index = data_indexes[current_data_index];
            data_batch.index_put_({i}, data.index({index}));
            label_batch.index_put_({i}, data_labels.index({index}));
            current_data_index++;
        }

        //Push batch onto batches
        batches.push_back(std::pair<torch::Tensor, torch::Tensor>(data_batch, 
                                                                  label_batch));
    }

    return batches;

}

} // namespace NeuroEvo
