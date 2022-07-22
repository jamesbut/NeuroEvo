#include <util/models/unsupervised_models/autoencoder.h>
#include <util/torch/tensor_utils.h>
#include <util/torch/weight_initialisation.h>
#include <util/torch/module_utils.h>

namespace NeuroEvo {

AutoEncoder::AutoEncoder(NetworkBuilder encoder_builder,
                         NetworkBuilder decoder_builder,
                         const double learning_rate,
                         const bool tied_weights,
                         const std::optional<const double> denoising_sigma) :
   TrainableModel(decoder_builder, "ae"),
    _encoder(dynamic_cast<TorchNetwork*>(encoder_builder.build_network())),
    _autoencoder(*_encoder, *_model),
    _learning_rate(learning_rate),
    _denoising_sigma(denoising_sigma),
    _tied_weights(tied_weights)
    {
        //set_module_weights_kaiming_uniform(*_model);

        if(_tied_weights)
            tie_weights();

    }

AutoEncoder::AutoEncoder(const JSON& config) :
    AutoEncoder(
        create_encoder_builder(config),
        create_decoder_builder(config),
        config.get<const double>({"learning_rate"}),
        config.value({"tied_weights"}, false),
        config.optional_value<const double>({"denoising_sigma"})
    ) {}

bool AutoEncoder::train(const unsigned num_epochs, const unsigned batch_size,
                        const torch::Tensor& training_data,
                        const std::optional<const torch::Tensor>& test_data,
                        const double weight_decay, const bool trace,
                        const unsigned test_every)
{
    //std::cout << _autoencoder->parameters() << std::endl;

    torch::optim::Adam optimizer(
        _autoencoder->parameters(),
        //_model->parameters(),
        torch::optim::AdamOptions(_learning_rate).weight_decay(weight_decay)
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

    /*
    //Local min checker
    const unsigned test_epoch = 200;
    const double plateau_loss = 5.;
    LocalMinChecker local_min_checker(test_epoch, plateau_loss);

    //Learning rate annealer
    const unsigned step_size = 500;
    const double gamma = 0.5;
    const bool verbosity = true;
    torch::optim::StepLR lr_scheduler(optimizer, step_size, gamma, verbosity);
    */

    torch::Tensor avg_test_loss = torch::zeros({1}, {torch::kFloat64});

    for(unsigned i = 0; i < num_epochs; i++)
    {

        /*
        //Dump decoder
        if(i % test_every == 0)
            write(i);
        */

        std::vector<std::pair<torch::Tensor, torch::Tensor>> batches;

        //If denoising autoencoder, apply noise to input
        if(_denoising_sigma.has_value())
        {
            const torch::Tensor corrupted_training_data =
                apply_gaussian_noise(training_data, _denoising_sigma.value());
            batches = generate_batches(batch_size, corrupted_training_data,
                                       training_data);
        } else
            batches = generate_batches(batch_size, training_data, training_data);

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

        torch::Tensor avg_loss = total_loss / training_data.size(0);

        // Test on test set
        if(((i+1) % test_every == 0) && test_data.has_value())
        {
            const auto [test_output, test_code] = ae_forward(test_data.value());
            const auto test_loss = loss_function(test_output, test_data.value());
            avg_test_loss = test_loss / test_data->size(0);
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
        throw std::length_error("The number of weights in the encoder and decoder are "
            "not equal\n Cannot tie weights in Autoencoder!");

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

NetworkBuilder AutoEncoder::create_encoder_builder(JSON config) const
{
    JSON encoder_config = config.at({"EncoderSpec"});

    //Add number of inputs and outputs for AE encoder
    encoder_config.emplace("num_inputs", config.at({"data_vec_size"}));
    encoder_config.emplace("num_outputs", config.at({"code_size"}));

    //Add sigmoid activation spec if none is given
    if(!encoder_config.has_value({"final_layer_activation_function"}))
    {
        JSON sigmoid_json;
        sigmoid_json.emplace("name", "SigmoidSpec");
        encoder_config.emplace("final_layer_activation_function", sigmoid_json);
    }

    NetworkBuilder encoder_builder(encoder_config);
    encoder_builder.make_torch_net();

    return encoder_builder;
}

NetworkBuilder AutoEncoder::create_decoder_builder(JSON config) const
{
    JSON decoder_config = config.at({"DecoderSpec"});

    //Add number of inputs and outputs for AE encoder
    decoder_config.emplace("num_inputs", config.at({"code_size"}));
    decoder_config.emplace("num_outputs", config.at({"data_vec_size"}));

    //Add linear activation function to final layer
    JSON linear_json;
    linear_json.emplace("name", "LinearSpec");
    decoder_config.emplace("final_layer_activation_function", linear_json);

    NetworkBuilder decoder_builder(decoder_config);
    decoder_builder.make_torch_net();

    return decoder_builder;
}

} // namespace NeuroEvo
