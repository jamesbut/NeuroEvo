#include <ATen/Functions.h>
#include <torch/nn/functional/loss.h>
#include <util/algorithms/vae.h>
#include <util/torch_utils.h>

namespace NeuroEvo {

//When handing encoder builder on does not have to define the separate fully connected
//layers that go separately to create mean and stddev - the constructor will do this
VAE::VAE(NetworkBuilder& encoder_builder,
         NetworkBuilder& decoder_builder,
         const torch::Tensor& training_data, 
         const torch::Tensor test_data,
         std::unique_ptr<Distribution<double>> init_net_weight_distr) :
    _training_data(training_data),
    _test_data(test_data),
    _encoder(build_torch_network(encoder_builder, std::move(init_net_weight_distr))),
    _decoder(build_torch_network(decoder_builder, std::move(init_net_weight_distr))),
    _encoder_mean_linear_layer(torch::nn::LinearOptions(_encoder->get_num_outputs(),
                                                        _decoder->get_num_inputs())),
    _encoder_logvar_linear_layer(torch::nn::LinearOptions(_encoder->get_num_outputs(),
                                                          _decoder->get_num_inputs())) {

    //TODO: Check networks are appropriate

}

void VAE::train(const unsigned num_epochs, const unsigned batch_size,
                const unsigned test_every)
{

    const double learning_rate = 1e-3;
    torch::optim::Adam optimizer(
        {_encoder->parameters(),
         _encoder_mean_linear_layer->parameters(),
         _encoder_logvar_linear_layer->parameters(),
         _decoder->parameters()},
         torch::optim::AdamOptions(learning_rate)
    ); 

    /*
    //optimizer.add_parameters(_decoder->parameters());
    std::cout << "Optimize size: " << optimizer.size() << std::endl;

    std::cout << "Optimizer params: " << std::endl;
    std::cout << optimizer.parameters() << std::endl;
    //for(const auto& param : optimizer.parameters())
    //    std::cout << param << std::endl;
    std::cout << "----------------" << std::endl;

    std::cout << "Encoder params:" << std::endl;
    std::cout << _encoder->parameters() << std::endl;
    std::cout << _encoder_mean_linear_layer->parameters() << std::endl;
    std::cout << _encoder_logvar_linear_layer->parameters() << std::endl;
    std::cout << "Decoder params: " << std::endl;
    std::cout << _decoder->parameters() << std::endl;
    */

    for(unsigned i = 0; i < num_epochs; i++)
    {

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> batches =
            generate_batches(batch_size, _training_data, _training_data);

        torch::Tensor total_loss = torch::zeros(1);

        for(const auto& batch : batches)
        {

            optimizer.zero_grad();

            const auto [output, mu, log_var] = forward(batch.first);

            torch::Tensor loss = loss_function(output, batch.second, mu, log_var);

            loss.backward();
            total_loss += loss;

            optimizer.step();
            
        }

        auto avg_loss = total_loss / _training_data.size(0);

        std::cout << "Epoch: " << i << " | Training Loss: " << avg_loss.item<float>() << std::endl;

    }
    
}

std::pair<torch::Tensor, torch::Tensor> VAE::encode(const torch::Tensor& x) 
{
    const torch::Tensor hidden_layer_out = _encoder->forward(x);
    const torch::Tensor mu = _encoder_mean_linear_layer->forward(hidden_layer_out);
    const torch::Tensor log_var = _encoder_logvar_linear_layer->forward(hidden_layer_out);
    return {mu, log_var};
}

torch::Tensor VAE::decode(const torch::Tensor& z) const
{
    return _decoder->forward(z);
}

std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> VAE::forward(const torch::Tensor& x,
                                                                     const bool trace)
{
    const auto [mu, log_var] = encode(x);
    if(trace)
        std::cout << "Mu: " << std::endl << mu << std::endl << "Log var: " << std::endl << 
            log_var << std::endl;

    const auto z = sample(mu, log_var, trace);
    if(trace)
        std::cout << "Sampled z: " << std::endl << z << std::endl;

    return std::make_tuple(decode(z), mu, log_var);
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
    const torch::Tensor eps = torch::randn({mu.size(0), mu.size(1)});

    //Sample from the distribution
    return eps * stddev + mu;
}

torch::Tensor VAE::loss_function(const torch::Tensor& output, const torch::Tensor& input,
                                 const torch::Tensor& mu, const torch::Tensor& log_var)
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
        torch::nn::functional::BinaryCrossEntropyFuncOptions().reduction(torch::kSum)
    );
    */

    //https://arxiv.org/abs/1312.6114
    //I think this is difference between the output gauss params and a unit normal
    torch::Tensor KLD_loss = -0.5 * torch::sum(1 + log_var - mu.pow(2) - log_var.exp());
    KLD_loss /= output.size(0);

    return reconstruction_loss + KLD_loss;

}

const std::unique_ptr<TorchNetwork>& VAE::get_decoder() const
{
    return _decoder;
}

} // namespace NeuroEvo
