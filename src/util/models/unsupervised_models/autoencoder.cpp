#include <util/models/unsupervised_models/autoencoder.h>

namespace NeuroEvo {

AutoEncoder::AutoEncoder(NetworkBuilder& encoder_builder,
                         NetworkBuilder& decoder_builder,
                         const torch::Tensor& training_data,  
                         const std::optional<const torch::Tensor>& test_data) :
    TrainableModel(training_data, test_data, decoder_builder, "ie_ae.pt"),
    _encoder(dynamic_cast<TorchNetwork*>(encoder_builder.build_network())),
    _autoencoder(*_encoder, *_model) {}

void AutoEncoder::train(const unsigned num_epochs, const unsigned batch_size,
                        const double weight_decay, const bool trace, 
                        const unsigned test_every) 
{

    const double learning_rate = 1e-3;

    //std::cout << _autoencoder->parameters() << std::endl;
    //exit(0);

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

    torch::Tensor avg_test_loss = torch::zeros({1}, {torch::kFloat64});

    for(unsigned i = 0; i < num_epochs; i++)
    {
        
        //Dump decoder 
        if(i % test_every == 0)
            write_model(i);
        //exit(0);

        const std::vector<std::pair<torch::Tensor, torch::Tensor>> batches =
            generate_batches(batch_size, _training_data, _training_data);

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
    }
    std::cout << _autoencoder->parameters() << std::endl;

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

} // namespace NeuroEvo
