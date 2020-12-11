#include <util/torch_utils.h>
#include <util/models/generative_models/autoencoder.h>

namespace NeuroEvo {

AutoEncoder::AutoEncoder(NetworkBuilder& encoder_builder,
                         NetworkBuilder& decoder_builder,
                         const torch::Tensor& training_data,  
                         const std::optional<const torch::Tensor>& test_data,
                         Distribution<double>* init_net_weight_distr) :
    GenerativeModel(training_data, test_data, "ie_ae.pt"),
    _encoder(build_torch_network(encoder_builder, init_net_weight_distr)),
    _decoder(build_torch_network(decoder_builder, init_net_weight_distr)),
    _autoencoder(*_encoder, *_decoder) {}

void AutoEncoder::train(const unsigned num_epochs, const unsigned batch_size,
                        const double weight_decay, const bool trace, 
                        const unsigned test_every) 
{

    const double learning_rate = 1e-3;
    auto adam_options = torch::optim::AdamOptions(learning_rate);
    adam_options.weight_decay(weight_decay);
    torch::optim::Adam optimizer(
        _autoencoder->parameters(), 
        adam_options
    );

    torch::Tensor avg_test_loss = torch::zeros({1}, {torch::kFloat64});

    for(unsigned i = 0; i < num_epochs; i++)
    {

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> batches =
            generate_batches(batch_size, _training_data, _training_data);

        torch::Tensor total_loss = torch::zeros(1, {torch::kFloat64});

        for(const auto& batch : batches)
        {
            _autoencoder->zero_grad();
            const auto [output, code] = forward(batch.first);

            torch::Tensor loss = loss_function(output, batch.second);

            loss.backward();
            total_loss += loss;

            optimizer.step();

        }

        torch::Tensor avg_loss = total_loss / _training_data.size(0);

        // Test on test set
        if(((i+1) % test_every == 0) && _test_data.has_value())
        {
            const auto [test_output, test_code] = forward(_test_data.value());
            const auto test_loss = loss_function(test_output, _test_data.value());
            avg_test_loss = test_loss / _test_data->size(0);
        }

        //Dump decoder 
        if((i+1) % test_every == 0)
            write_decoder(i+1);

        std::cout << "Epoch: " << i << " | Training Loss: " << avg_loss.item<double>() 
            << " | Test Loss: " << avg_test_loss.item<double>() << std::endl;

    }

}

torch::Tensor AutoEncoder::encode(const torch::Tensor& x) const
{
    return _encoder->forward(x);
}

torch::Tensor AutoEncoder::generate(const torch::Tensor& x) const
{
    return _decoder->forward(x);
}

std::tuple<torch::Tensor, torch::Tensor> AutoEncoder::forward(const torch::Tensor &x)
{
    const auto code = _encoder->forward(x);
    return std::make_tuple(_decoder->forward(code), code);
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

const std::unique_ptr<TorchNetwork>& AutoEncoder::get_decoder() const
{
    return _decoder;
}

} // namespace NeuroEvo
