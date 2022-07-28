#include <torch/nn/functional/loss.h>
#include <util/models/unsupervised_models/vae.h>

namespace NeuroEvo {

//When handing encoder builder one does not have to define the separate fully
//connected layers that go separately to create mean and stddev - the constructor
//will do this
VAE::VAE(std::optional<NetworkBuilder> encoder_builder,
         NetworkBuilder decoder_builder) :
    TrainableModel(decoder_builder, "vae"),
    _encoder(
        encoder_builder ?
        dynamic_cast<TorchNetwork*>(encoder_builder->build_network()) : nullptr),
    _encoder_mean_linear_layer(
        _encoder ?
        torch::nn::LinearOptions(_encoder->get_num_outputs(),
                                 _model->get_num_inputs()) :
        torch::nn::LinearOptions(_model->get_num_outputs(),
                                 _model->get_num_inputs())),
    _encoder_logvar_linear_layer(
        _encoder ?
        torch::nn::LinearOptions(_encoder->get_num_outputs(),
                                 _model->get_num_inputs()) :
        torch::nn::LinearOptions(_model->get_num_outputs(),
                                 _model->get_num_inputs()))
    {
        _encoder_mean_linear_layer->to(torch::kFloat64);
        _encoder_logvar_linear_layer->to(torch::kFloat64);
    }

VAE::VAE(const JSON& config) :
    VAE(create_encoder_builder(config), create_decoder_builder(config)) {}

bool VAE::train(const unsigned num_epochs, const unsigned batch_size,
                const torch::Tensor& training_data,
                const std::optional<const torch::Tensor>& test_data,
                const double weight_decay, const bool trace,
                const unsigned test_every)
{

    //TODO: move to config
    const double learning_rate = 5e-4;
    auto adam_options = torch::optim::AdamOptions(learning_rate);
    adam_options.weight_decay(weight_decay);

    std::vector<torch::optim::OptimizerParamGroup> opt_params;
    if(_encoder)
        opt_params.push_back(_encoder->parameters());
    opt_params.push_back(_encoder_mean_linear_layer->parameters());
    opt_params.push_back(_encoder_logvar_linear_layer->parameters());
    opt_params.push_back(_model->parameters());

    torch::optim::Adam optimizer(
         opt_params,
         adam_options
    );

    for(unsigned i = 0; i < num_epochs; i++)
    {

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> batches =
            generate_batches(batch_size, training_data, training_data);

        torch::Tensor total_loss = torch::zeros(1, {torch::kFloat64});

        for(const auto& batch : batches)
        {

            optimizer.zero_grad();

            const auto [output, mu, log_var] = vae_forward(batch.first);

            torch::Tensor loss = loss_function(output, batch.second, mu, log_var);

            loss.backward();
            total_loss += loss;

            optimizer.step();

        }

        auto avg_loss = total_loss / training_data.size(0);

        std::cout << "Epoch: " << i << " | Training Loss: "
            << avg_loss.item<double>() << std::endl;

        _loss = avg_loss.item<double>();
    }

    return true;

}

std::pair<torch::Tensor, torch::Tensor> VAE::encode(const torch::Tensor& x)
{
    torch::Tensor hidden_layer_out = x;
    if(_encoder)
        hidden_layer_out = _encoder->forward(x);
    const torch::Tensor mu = _encoder_mean_linear_layer->forward(hidden_layer_out);
    const torch::Tensor log_var =
        _encoder_logvar_linear_layer->forward(hidden_layer_out);
    return {mu, log_var};
}

std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>
    VAE::vae_forward(const torch::Tensor& x,
                     const bool trace)
{
    const auto [mu, log_var] = encode(x);
    if(trace)
        std::cout << "Mu: " << std::endl << mu << std::endl << "Log var: "
            << std::endl << log_var << std::endl;

    const auto z = sample(mu, log_var, trace);
    if(trace)
        std::cout << "Sampled z: " << std::endl << z << std::endl;

    return std::make_tuple(_model->forward(z), mu, log_var);
}

//Sample from the distribution with mu and logvar params
torch::Tensor VAE::sample(const torch::Tensor& mu, const torch::Tensor& log_var,
                          const bool trace) const
{

    //Calculate stddev
    const torch::Tensor stddev = torch::exp(0.5 * log_var);
    if(trace)
        std::cout << "Stddev: " << std::endl << stddev << std::endl;

    //Randomly generate epsilons from unit normal distribution
    const torch::Tensor eps = torch::randn({mu.size(0), 
                                            mu.size(1)},
                                           {torch::kFloat64});

    //Sample from the distribution
    return eps * stddev + mu;
}

torch::Tensor VAE::loss_function(const torch::Tensor& output,
                                 const torch::Tensor& input,
                                 const torch::Tensor& mu,
                                 const torch::Tensor& log_var) const
{

    //The difference between the input and the output (reconstruction loss)
    torch::Tensor reconstruction_loss = torch::nn::functional::mse_loss(
        output,
        input,
        torch::nn::functional::MSELossFuncOptions().reduction(torch::kSum)
    );

    //The difference between the input and the output (reconstruction loss)
    /*
    const torch::Tensor BCE_loss = torch::nn::functional::binary_cross_entropy(
        output,
        input,
        torch::nn::functional::BinaryCrossEntropyFuncOptions()
            .reduction(torch::kSum)
    );
    */

    //https://arxiv.org/abs/1312.6114
    //I think this is difference between the output gauss params and a unit normal
    torch::Tensor KLD_loss = -0.5 *
        torch::sum(1 + log_var - mu.pow(2) - log_var.exp());
    KLD_loss /= output.size(0);

    //std::cout << "Reconstruction loss: " << reconstruction_loss << std::endl;
    //std::cout << "KLD loss:" << KLD_loss << std::endl;

    return reconstruction_loss + KLD_loss;

}

std::optional<NetworkBuilder> VAE::create_encoder_builder(JSON config) const
{
    JSON encoder_config = config.at({"EncoderSpec"});

    if(encoder_config.at({"num_hidden_layers"}) == 0)
        return std::nullopt;
    else
    {
        //Add number of inputs and outputs for VAE encoder
        encoder_config.emplace("num_inputs", config.at({"data_vec_size"}));
        encoder_config.emplace("num_outputs",
                               encoder_config.at({"neurons_per_hidden_layer"}));

        //Alter number of hidden layers and neurons per hidden layer
        const unsigned altered_num_hidden_layers =
            encoder_config.get<int>({"num_hidden_layers"}) <= 0 ?
                0 : encoder_config.get<int>({"num_hidden_layers"})-1;
        encoder_config.emplace("num_hidden_layers", altered_num_hidden_layers);

        if(altered_num_hidden_layers == 0)
            encoder_config.emplace("neurons_per_hidden_layer", 0);

        //Default activation function is ReLU for both hidden and final layers
        JSON relu_json;
        relu_json.emplace({"name"}, "ReLUSpec");

        if(!encoder_config.has_value({"hidden_layer_activation_function"}))
            encoder_config.emplace("hidden_layer_activation_function", relu_json);
        if(!encoder_config.has_value({"final_layer_activation_function"}))
            encoder_config.emplace("final_layer_activation_function", relu_json);

        NetworkBuilder encoder_builder(encoder_config);
        encoder_builder.make_torch_net();

        return encoder_builder;
    }
}

NetworkBuilder VAE::create_decoder_builder(JSON config) const
{
    JSON decoder_config = config.at({"DecoderSpec"});

    //Add number of inputs and outputs for VAE decoder
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
