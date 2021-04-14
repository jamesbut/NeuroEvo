#include <util/models/unsupervised_models/autoencoder.h>
#include <util/torch/tensor_utils.h>
#include <util/torch/weight_initialisation.h>
#include <util/torch/module_utils.h>

namespace NeuroEvo {

AutoEncoder::AutoEncoder(NetworkBuilder& encoder_builder,
                         NetworkBuilder& decoder_builder,
                         const torch::Tensor& training_data,
                         const std::optional<const torch::Tensor>& test_data,
                         const bool tied_weights,
                         const std::optional<const double> denoising_sigma) :
    TrainableModel(training_data, test_data, decoder_builder, "ie_ae.pt"),
    _encoder(dynamic_cast<TorchNetwork*>(encoder_builder.build_network())),
    _autoencoder(*_encoder, *_model),
    _denoising_sigma(denoising_sigma),
    _tied_weights(tied_weights)
    {
        //set_module_weights_kaiming_uniform(*_model);

        if(_tied_weights)
            tie_weights();

    }

bool AutoEncoder::train(const unsigned num_epochs, const unsigned batch_size,
                        const double weight_decay, const bool trace,
                        const unsigned test_every)
{
    //std::cout << _autoencoder->parameters() << std::endl;

    const double learning_rate = 1e-3;
    //const double learning_rate = 1e-2;

    torch::optim::Adam optimizer(
        _autoencoder->parameters(),
        //_model->parameters(),
        torch::optim::AdamOptions(learning_rate).weight_decay(weight_decay)
    );

    /*
    const double momentum = 0.5;
    torch::optim::SGD optimizer(
        _autoencoder->parameters(),
        torch::optim::SGDOptions(learning_rate).momentum(momentum).nesterov(true)
    );

    torch::optim::RMSprop optimizer(
        _autoencoder->parameters(),
        torch::optim::RMSpropOptions(learning_rate)
    );
    */

    //Local min checker
    const unsigned test_epoch = 200;
    const double plateau_loss = 5.;
    LocalMinChecker local_min_checker(test_epoch, plateau_loss);

    //Learning rate annealer
    const unsigned step_size = 500;
    const double gamma = 0.5;
    const bool verbosity = true;
    torch::optim::StepLR lr_scheduler(optimizer, step_size, gamma, verbosity);

    torch::Tensor avg_test_loss = torch::zeros({1}, {torch::kFloat64});

    for(unsigned i = 0; i < num_epochs; i++)
    {

        //Dump decoder
        if(i % test_every == 0)
            write_model(i);

        std::vector<std::pair<torch::Tensor, torch::Tensor>> batches;

        //If denoising autoencoder, apply noise to input
        if(_denoising_sigma.has_value())
        {
            const torch::Tensor corrupted_training_data =
                apply_gaussian_noise(_training_data, _denoising_sigma.value());
            batches = generate_batches(batch_size, corrupted_training_data,
                                       _training_data);
        } else
            batches = generate_batches(batch_size, _training_data, _training_data);

        torch::Tensor total_loss = torch::zeros(1, {torch::kFloat64});

        for(const auto& batch : batches)
        {
            _autoencoder->zero_grad();
            const auto [output, code] = ae_forward(batch.first);

            torch::Tensor loss = loss_function(output, batch.second);

            loss.backward();
            total_loss += loss;

            optimizer.step();

        }

        torch::Tensor avg_loss = total_loss / _training_data.size(0);

        // Test on test set
        if(((i+1) % test_every == 0) && _test_data.has_value())
        {
            const auto [test_output, test_code] = ae_forward(_test_data.value());
            const auto test_loss = loss_function(test_output, _test_data.value());
            avg_test_loss = test_loss / _test_data->size(0);
        }


        std::cout << "Epoch: " << i << " | Training Loss: " << avg_loss.item<double>()
            << " | Test Loss: " << avg_test_loss.item<double>() << std::endl;

        _loss = avg_loss.item<double>();

        /*
        //Local min checker
        if(local_min_checker.is_in_local_min(i, _loss))
            return false;
        else
            //But also for now return if not in local min but one has reached
            //the test epoch - this is only to make local min testing faster
            //I will remove this in future
            if(i >= test_epoch)
                return true;
        */

        //lr_scheduler.step();

    }

    //std::cout << _encoder->parameters() << std::endl;
    //std::cout << _autoencoder->parameters() << std::endl;

    return true;

}

torch::Tensor AutoEncoder::encode(const torch::Tensor& x) const
{
    return _encoder->forward(x);
}

std::tuple<torch::Tensor, torch::Tensor> AutoEncoder::ae_forward(const torch::Tensor &x)
{
    const auto code = _encoder->forward(x);
    return std::make_tuple(_model->forward(code), code);
}

torch::Tensor AutoEncoder::loss_function(const torch::Tensor& output,
                                         const torch::Tensor& input) const
{

    torch::Tensor loss = torch::nn::functional::mse_loss(
        output,
        input,
        torch::nn::functional::MSELossFuncOptions().reduction(torch::kSum)
    );

    return loss;

}

void AutoEncoder::tie_weights()
{

    //Check number of weights in encoder and decoder are equal
    if(count_num_weights_linear(*_encoder) != count_num_weights_linear(*_model))
    {
        std::cout << "The number of weights in the encoder and decoder are not equal"
            << std::endl << "Cannot tie weights in Autoencoder!" << std::endl;
        exit(0);
    }

    /* Set transposed weights in the decoder */

    //Isolate the linear layers and then iterate over them
    std::vector<std::shared_ptr<torch::nn::Module>> e_linear_modules =
        get_linear_layers(*_encoder);
    std::vector<std::shared_ptr<torch::nn::Module>> d_linear_modules =
        get_linear_layers(*_model);

    for(std::size_t i = 0; i < e_linear_modules.size(); i++)
    {
        //Cast to Linear modules
        auto* e_linear = e_linear_modules.at(i)->as<torch::nn::Linear>();
        auto* d_linear =
            d_linear_modules.at(e_linear_modules.size()-1-i)->as<torch::nn::Linear>();
        //Set transposition
        d_linear->weight.set_data(e_linear->weight.transpose(0, 1));
    }

}

} // namespace NeuroEvo
