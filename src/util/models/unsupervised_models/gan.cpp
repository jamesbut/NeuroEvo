#include <torch/data/dataloader.h>
#include <torch/nn/functional/loss.h>
#include <torch/nn/options/loss.h>
#include <torch/optim/sgd.h>
#include <util/models/unsupervised_models/gan.h>
#include <util/statistics/distributions/gaussian_distribution.h>
#include <util/formatting.h>
#include <util/torch/tensor_utils.h>
#include <util/number_ranges.h>

namespace NeuroEvo {

GAN::GAN(NetworkBuilder& generator_builder,
         NetworkBuilder& discriminator_builder,
         const torch::Tensor& real_data) :
    TrainableModel(real_data, std::nullopt, generator_builder, "ie_gan.pt"),
    _discriminator(dynamic_cast<TorchNetwork*>(discriminator_builder.build_network())) {}

bool GAN::train(const unsigned num_epochs, const unsigned batch_size,
                const double weight_decay, const bool trace,
                const unsigned test_every)
{

    //This was part of the tutorial but stop loss reducing to zero - maybe it is important
    //for a GAN
    //torch::Tensor real_labels = torch::empty(_real_data.size(0)).uniform_(0.8, 1.0);
    //torch::Tensor real_labels = torch::ones(_real_data.size(0));
    torch::Tensor real_labels = torch::ones({_training_data.size(0), 1},
                                            {torch::kFloat64});

    //const double generator_learning_rate = 2e-4;
    //const double discriminator_learning_rate = 5e-4;
    const double generator_learning_rate = 1e-4;
    const double discriminator_learning_rate = 1e-4;

    //const double beta_1 = 0.9;
    //const double beta_2 = 0.999;

    /*
    torch::optim::Adam generator_optimizer(
        _generator->parameters(),
        torch::optim::AdamOptions(generator_learning_rate)
            //.betas(std::make_tuple(beta_1, beta_2))
    );
    torch::optim::Adam discriminator_optimizer(
        _discriminator->parameters(),
        torch::optim::AdamOptions(discriminator_learning_rate)
            //.betas(std::make_tuple(beta_1, beta_2))
    );
    */

    /*
    torch::optim::SGD generator_optimizer(
        _generator->parameters(),
        torch::optim::SGDOptions(generator_learning_rate)
    );
    torch::optim::SGD discriminator_optimizer(
        _discriminator->parameters(),
        torch::optim::SGDOptions(discriminator_learning_rate)
    );
    */

    torch::optim::RMSprop generator_optimizer(
        _model->parameters(),
        torch::optim::RMSpropOptions(generator_learning_rate)
    );
    torch::optim::RMSprop discriminator_optimizer(
        _discriminator->parameters(),
        torch::optim::RMSpropOptions(discriminator_learning_rate)
    );

    const unsigned column_width = 20;
    const std::vector<std::string> header_data{"Epoch",
                                               "Discriminator loss",
                                               "Generator loss",
                                               "Generator symmetry"};
    print_table_row(header_data, column_width, true, true);

    double generator_symmetry = 0.;

    for(unsigned i = 0; i < num_epochs; i++)
    {
        //Dump decoder
        if(i % test_every == 0)
            write_model(i);

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> real_batches =
            generate_batches(batch_size, _training_data, real_labels);

        torch::Tensor total_d_loss = torch::zeros(1, {torch::kFloat64});
        torch::Tensor total_g_loss = torch::zeros(1, {torch::kFloat64});

        for(const auto& real_batch : real_batches)
        {

            /* Train discriminator on real data */
            _discriminator->zero_grad();
            torch::Tensor d_real_output = _discriminator->forward(real_batch.first);
            //BCE loss with summed reduction (just sums the loss for the batch)
            torch::Tensor d_real_loss = torch::nn::functional::binary_cross_entropy(
                d_real_output,
                real_batch.second,
                torch::nn::functional::BinaryCrossEntropyFuncOptions()
                    .reduction(torch::kSum)
            );
            d_real_loss.backward();

            /* Train discriminator on fake data */
            //Generate fake data using generator
            //Draw noise from unit normal distribution
            auto noise = torch::randn({real_batch.first.size(0),
                                       _model->get_num_inputs()},
                                      {torch::kFloat64});
            //Draw noise from a uniform distribution [0,1]
            //auto noise = torch::rand({real_batch.first.size(0),
            //                          _generator->get_num_inputs()},
            //                         {torch::kFloat64});

            torch::Tensor fake_data = _model->forward(noise);

            torch::Tensor d_fake_output = _discriminator->forward(fake_data.detach());
            torch::Tensor fake_labels = torch::zeros({fake_data.size(0), 1},
                                                     {torch::kFloat64});
            torch::Tensor d_fake_loss = torch::nn::functional::binary_cross_entropy(
                d_fake_output,
                fake_labels,
                torch::nn::functional::BinaryCrossEntropyFuncOptions()
                    .reduction(torch::kSum)
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
            _model->zero_grad();
            fake_labels.fill_(1);
            torch::Tensor d_output = _discriminator->forward(fake_data);
            torch::Tensor g_loss = torch::nn::functional::binary_cross_entropy(
                d_output,
                fake_labels,
                torch::nn::functional::BinaryCrossEntropyFuncOptions()
                    .reduction(torch::kSum)
            );
            total_g_loss += g_loss;
            g_loss.backward();
            generator_optimizer.step();


        }

        //Test generator
        if((i+1) % test_every == 0)
        {
            const bool random_noise = true;
            generator_symmetry = test_generator_symmetry(random_noise);
        }

        //Divide by total number of data points seen
        torch::Tensor avg_d_loss = total_d_loss / (_training_data.size(0) * 2);
        //Divide by size of fake data
        torch::Tensor avg_g_loss = total_g_loss / _training_data.size(0);

        const std::vector<double> row_data{static_cast<double>(i),
                                           avg_d_loss.item<double>(),
                                           avg_g_loss.item<double>(),
                                           generator_symmetry};
        print_table_row(row_data, column_width);

    }

    return true;

}

torch::Tensor GAN::discriminate(const torch::Tensor& x) const
{
    return _discriminator->forward(x);
}

double GAN::test_generator_symmetry(const bool random_noise) const
{
    //Generate latent space data
    //Draw noise from unit normal distribution
    const unsigned noise_size = 100;
    torch::Tensor noise;
    if(random_noise || (_model->get_num_inputs() > 1))
        noise = torch::randn({noise_size, _model->get_num_inputs()},
                             {torch::kFloat64});
    else
        noise = create_range_torch(-2., 2., 0.01);

    //Create noise range
    torch::Tensor generator_out = _model->forward(noise);
    //const double symmetry = measure_symmetry(generator_out);
    const double symmetry = measure_square_symmetry(generator_out);
    return symmetry;
}

} // namespace NeuroEvo
