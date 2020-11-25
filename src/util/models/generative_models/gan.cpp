#include <ATen/Functions.h>
#include <ATen/core/TensorBody.h>
#include <torch/data/dataloader.h>
#include <torch/nn/functional/loss.h>
#include <torch/nn/options/loss.h>
#include <util/models/generative_models/gan.h>
#include <util/statistics/distributions/gaussian_distribution.h>
#include <util/torch_utils.h>

namespace NeuroEvo {

GAN::GAN(NetworkBuilder& generator_builder,
         NetworkBuilder& discriminator_builder, 
         const torch::Tensor& real_data,
         Distribution<double>* init_net_weight_distr) :
    GenerativeModel(real_data, std::nullopt),
    _generator(build_torch_network(generator_builder, init_net_weight_distr)),
    _discriminator(build_torch_network(discriminator_builder, init_net_weight_distr)) {}

void GAN::train(const unsigned num_epochs, const unsigned batch_size, 
                const double weight_decay, const bool trace, 
                const unsigned test_every)
{
    
    //This was part of the tutorial but stop loss reducing to zero - maybe it is important
    //for a GAN
    //torch::Tensor real_labels = torch::empty(_real_data.size(0)).uniform_(0.8, 1.0);
    //torch::Tensor real_labels = torch::ones(_real_data.size(0));
    torch::Tensor real_labels = torch::ones({_training_data.size(0), 1});

    const double generator_learning_rate = 2e-4;
    const double discriminator_learning_rate = 5e-4;

    //The tutorial uses betas on the following AdamOptions
    torch::optim::Adam generator_optimizer(
        _generator->parameters(), torch::optim::AdamOptions(generator_learning_rate)
    );
    torch::optim::Adam discriminator_optimizer(
        _discriminator->parameters(), torch::optim::AdamOptions(discriminator_learning_rate)
    );

    for(unsigned i = 0; i < num_epochs; i++)
    {

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> real_batches =
            generate_batches(batch_size, _training_data, real_labels);

        torch::Tensor total_d_loss = torch::zeros(1);
        torch::Tensor total_g_loss = torch::zeros(1);

        for(const auto& real_batch : real_batches)
        {

            /* Train discriminator on real data */
            _discriminator->zero_grad();
            torch::Tensor d_real_output = _discriminator->forward(real_batch.first);
            //BCE loss with summed reduction (just sums the loss for the batch)
            torch::Tensor d_real_loss = torch::nn::functional::binary_cross_entropy(
                d_real_output, 
                real_batch.second,
                torch::nn::functional::BinaryCrossEntropyFuncOptions().reduction(torch::kSum)
            );
            d_real_loss.backward();

            /* Train discriminator on fake data */
            //Generate fake data using generator
            //Draw noise from unit normal distribution
            auto noise = torch::randn({real_batch.first.size(0), _generator->get_num_inputs()});
            //Draw noise from a uniform distribution [0,1]
            //auto noise = torch::rand({real_batch.first.size(0), _generator->get_num_inputs()});
            
            torch::Tensor fake_data = _generator->forward(noise);

            torch::Tensor d_fake_output = _discriminator->forward(fake_data.detach());
            torch::Tensor fake_labels = torch::zeros({fake_data.size(0), 1});
            torch::Tensor d_fake_loss = torch::nn::functional::binary_cross_entropy(
                d_fake_output, 
                fake_labels,
                torch::nn::functional::BinaryCrossEntropyFuncOptions().reduction(torch::kSum)
            );
            d_fake_loss.backward();

            total_d_loss += d_real_loss + d_fake_loss;
            discriminator_optimizer.step();

            /*
            if(i == 999)
            {
                std::cout << "Real data:" << std::endl;
                std::cout << real_batch.first << std::endl;
                std::cout << "Real data D outputs:" << std::endl;
                std::cout << d_real_output << std::endl;
                std::cout << "Fake data D outputs:" << std::endl;
                std::cout << d_fake_output << std::endl;
                std::cout << "G output:" << std::endl;
                std::cout << fake_data << std::endl;
            }
            */

            /* Train generator */
            _generator->zero_grad();
            fake_labels.fill_(1);
            torch::Tensor d_output = _discriminator->forward(fake_data);
            torch::Tensor g_loss = torch::nn::functional::binary_cross_entropy(
                d_output, 
                fake_labels,
                torch::nn::functional::BinaryCrossEntropyFuncOptions().reduction(torch::kSum)
            );
            total_g_loss += g_loss;
            g_loss.backward();
            generator_optimizer.step();
           

        }

        //Divide by total number of data points seen
        torch::Tensor avg_d_loss = total_d_loss / (_training_data.size(0) * 2);
        //Divide by size of fake data
        torch::Tensor avg_g_loss = total_g_loss / _training_data.size(0);
        
        std::cout << "Epoch: " << i << " | Discriminator loss: " 
            << avg_d_loss.item<float>() << " | Generator loss: "
            << avg_g_loss.item<float>() <<  std::endl;
 
    }

}

torch::Tensor GAN::discriminate(const torch::Tensor& x) const
{
    return _discriminator->forward(x);
}

torch::Tensor GAN::generate(const torch::Tensor& x) const 
{
    return _generator->forward(x);
}

const std::unique_ptr<TorchNetwork>& GAN::get_generator() const
{
    return get_decoder();
}

const std::unique_ptr<TorchNetwork>& GAN::get_decoder() const
{
    return _generator;
}

} // namespace NeuroEvo
