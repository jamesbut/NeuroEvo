#ifndef _AUTOENCODER_H_
#define _AUTOENCODER_H_

/*
 * This class defines an autoencoder using the PyTorch C++ API
 */

#include <phenotype/phenotype_specs/network_builder.h>
#include <util/models/generative_models/generative_model.h>

namespace NeuroEvo {

//Even though an AE is not technically a generative model, I am using it like that for the moment
class AutoEncoder : public GenerativeModel
{

public:

    AutoEncoder(NetworkBuilder& encoder_builder,
                NetworkBuilder& decoder_builder, 
                const torch::Tensor& training_data, 
                const std::optional<const torch::Tensor>& test_data = std::nullopt, 
                Distribution<double>* init_net_weight_distr = nullptr);

    void train(const unsigned num_epochs, const unsigned batch_size, 
               const double weight_decay = 0., const bool trace = false, 
               const unsigned test_every = 1e6) override;

    torch::Tensor encode(const torch::Tensor& x) const;
    //Better known in this context as decode
    torch::Tensor generate(const torch::Tensor& x) const override;
    std::tuple<torch::Tensor, torch::Tensor> forward(const torch::Tensor& x);

    const std::unique_ptr<TorchNetwork>& get_decoder() const override;

private:

    torch::Tensor loss_function(const torch::Tensor& output, const torch::Tensor& input) const;

    std::unique_ptr<TorchNetwork> _encoder;
    std::unique_ptr<TorchNetwork> _decoder;

    torch::nn::Sequential _autoencoder;

};

} // namespace NeuroEvo

#endif
