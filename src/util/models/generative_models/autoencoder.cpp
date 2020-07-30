#include <util/torch_utils.h>
#include <util/models/generative_models/autoencoder.h>

namespace NeuroEvo {

AutoEncoder::AutoEncoder(NetworkBuilder& encoder_builder,
                         NetworkBuilder& decoder_builder,
                         const torch::Tensor& training_data,  
                         const std::optional<const torch::Tensor>& test_data,
                         std::unique_ptr<Distribution<double>> init_net_weight_distr) :
    GenerativeModel(training_data, test_data),
    _encoder(build_torch_network(encoder_builder, std::move(init_net_weight_distr))),
    _decoder(build_torch_network(decoder_builder, std::move(init_net_weight_distr))),
    _autoencoder(*_encoder, *_decoder) {}

void AutoEncoder::train(const unsigned num_epochs, const unsigned batch_size,
                        const bool trace, const unsigned test_every) 
{

    const double learning_rate = 1e-3;
    torch::optim::Adam optimizer(
        _autoencoder->parameters(), torch::optim::AdamOptions(learning_rate)
    );

    torch::Tensor avg_test_loss = torch::zeros({1});

    for(unsigned i = 0; i < num_epochs; i++)
    {

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> batches =
            generate_batches(batch_size, _training_data, _training_data);

        torch::Tensor total_loss = torch::zeros(1);

        for(const auto& batch : batches)
        {
            _autoencoder->zero_grad();
            auto ae_output = _autoencoder->forward(batch.first);

            torch::Tensor loss = torch::nn::functional::mse_loss(
                ae_output, 
                batch.second,
                torch::nn::functional::MSELossFuncOptions().reduction(torch::kSum)
            );

            loss.backward();
            total_loss += loss;

            optimizer.step();

        }

        torch::Tensor avg_loss = total_loss / _training_data.size(0);

        /* Test on test set */
        if(((i+1) % test_every == 0) && _test_data.has_value())
        {
            auto test_output = forward(_test_data.value());
            torch::Tensor test_loss = torch::nn::functional::mse_loss(
                test_output, 
                _test_data.value(),
                torch::nn::functional::MSELossFuncOptions().reduction(torch::kSum)
            );

            avg_test_loss = test_loss / _test_data->size(0);
        }

        std::cout << "Epoch: " << i << " | Training Loss: " << avg_loss.item<float>() 
            << " | Test Loss: " << avg_test_loss.item<float>() << std::endl;

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

torch::Tensor AutoEncoder::forward(const torch::Tensor &x) const
{
    return _decoder->forward(_encoder->forward(x));
}

const std::unique_ptr<TorchNetwork>& AutoEncoder::get_decoder() const
{
    return _decoder;
}

} // namespace NeuroEvo
