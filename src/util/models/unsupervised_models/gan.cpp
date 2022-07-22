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

GAN::GAN(NetworkBuilder generator_builder,
         NetworkBuilder discriminator_builder,
         const std::optional<cGANParams>& cgan_params) :
    TrainableModel(generator_builder, "gan"),
    _discriminator(
        dynamic_cast<TorchNetwork*>(discriminator_builder.build_network())),
    _cgan_params(cgan_params) {}

GAN::GAN(const JSON& config) :
    GAN(create_generator_builder(config),
        create_discriminator_builder(config),
        std::nullopt) {}

bool GAN::train(const unsigned num_epochs, const unsigned batch_size,
                const torch::Tensor& train_data,
                const std::optional<const torch::Tensor>& test_data,
                const double weight_decay, const bool trace,
                const unsigned test_every)
{

    torch::Tensor training_data = train_data.clone().detach().set_requires_grad(true);

    //This was part of the tutorial but stop loss reducing to zero - maybe it is
    //important for a GAN
    //torch::Tensor real_labels = torch::empty(_real_data.size(0)).uniform_(0.8, 1.0);
    //torch::Tensor real_labels = torch::ones(_real_data.size(0));
    torch::Tensor real_labels = torch::ones({training_data.size(0), 1},
                                            {torch::kFloat64});

    //TODO: Move to JSON config
    const double generator_learning_rate = 2e-4;
    const double discriminator_learning_rate = 5e-4;
    //const double generator_learning_rate = 1e-4;
    //const double discriminator_learning_rate = 1e-4;

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


    //Concatenate conditional labels with training data if there is any
    if(_cgan_params.has_value())
        training_data = torch::cat({training_data,
                                    _cgan_params.value().conditional_labels},
                                    1);

    const unsigned column_width = 20;
    const std::vector<std::string> header_data{"Epoch",
                                               "Discriminator loss",
                                               "Generator loss",
                                               "Generator symmetry"};
    print_table_row(header_data, column_width, true, true);

    double generator_symmetry = 0.;

    for(unsigned i = 0; i < num_epochs; i++)
    {
        /*
        //Dump decoder
        if(i % test_every == 0)
            write_model(i);
        */

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> real_batches =
            generate_batches(batch_size, training_data, real_labels);

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

            //Do not generate noise for conditional inputs yet
            unsigned noise_size = _model->get_num_inputs();
            if(_cgan_params.has_value())
                noise_size -= _cgan_params->conditional_labels.size(1);

            //Draw noise from unit normal distribution
            auto noise = torch::randn({real_batch.first.size(0), noise_size},
                                      {torch::kFloat64});
            //Draw noise from a uniform distribution [0,1]
            //auto noise = torch::rand({real_batch.first.size(0),
            //                          _generator->get_num_inputs()},
            //                         {torch::kFloat64});

            //Draw noise for conditional input if cGAN
            std::optional<torch::Tensor> conditional_noise = std::nullopt;
            if(_cgan_params.has_value())
            {
                conditional_noise = draw_conditional_noise(real_batch.first.size(0));
                //Concatenate with normal noise
                noise = torch::cat({noise, conditional_noise.value()}, 1);
            }

            torch::Tensor fake_data = _model->forward(noise);

            //Concatenate conditional noise if cGAN
            if(conditional_noise.has_value())
                fake_data = torch::cat({fake_data, conditional_noise.value()}, 1);

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
        torch::Tensor avg_d_loss = total_d_loss / (training_data.size(0) * 2);
        //Divide by size of fake data
        torch::Tensor avg_g_loss = total_g_loss / training_data.size(0);

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
        noise = create_range_torch(Range(-2., 2., 0.01));

    //Create noise range
    torch::Tensor generator_out = _model->forward(noise);
    //const double symmetry = measure_symmetry(generator_out);
    const double symmetry = measure_square_symmetry(generator_out);
    return symmetry;
}

torch::Tensor GAN::draw_conditional_noise(const unsigned num_noise_vecs) const
{
    //Create noise in appropriate conditional bounds
    torch::Tensor noise = torch::rand({num_noise_vecs,
                                       _cgan_params->conditional_labels.size(1)},
                                      {torch::kFloat64});
    noise *= (_cgan_params->conditional_ub - _cgan_params->conditional_lb);
    noise += _cgan_params->conditional_lb;

    return noise;
}

NetworkBuilder GAN::create_generator_builder(JSON config) const
{
    JSON generator_config = config.at({"GeneratorSpec"});

    //TODO: Number of inputs for generator must also include the conditional label size

    //Add number of inputs and outputs for generator
    generator_config.emplace("num_inputs", config.at({"code_size"}));
    generator_config.emplace("num_outputs", config.at({"data_vec_size"}));

    //Add linear activation function to final layer
    JSON linear_json;
    linear_json.emplace("name", "LinearSpec");
    generator_config.emplace("final_layer_activation_function", linear_json);

    NetworkBuilder generator_builder(generator_config);
    generator_builder.make_torch_net();

    return generator_builder;
}

NetworkBuilder GAN::create_discriminator_builder(JSON config) const
{
    JSON discriminator_config = config.at({"DiscriminatorSpec"});

    //TODO: Number of inputs for discriminator must also include conditional label size

    //Add number of inputs and outputs for discriminator
    discriminator_config.emplace("num_inputs", config.at({"data_vec_size"}));
    discriminator_config.emplace("num_outputs", 1);

    //Sigmoid spec on the final layer for the discriminator
    JSON sigmoid_json;
    sigmoid_json.emplace("name", "SigmoidSpec");
    discriminator_config.emplace("final_layer_activation_function", sigmoid_json);

    NetworkBuilder discriminator_builder(discriminator_config);
    discriminator_builder.make_torch_net();

    return discriminator_builder;
}

} // namespace NeuroEvo
