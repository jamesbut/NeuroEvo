#ifndef _VAE_H_
#define _VAE_H_

/*
 * This class defines a Variational Autoencoder using the Torch C++ API
 */

#include <phenotype/phenotype_specs/network_builder.h>
#include <util/models/trainable_model.h>
#include <phenotype/neural_network/torch_network.h>

namespace NeuroEvo {

class VAE : public TrainableModel
{

public:

    VAE(std::optional<NetworkBuilder> encoder_builder,
        NetworkBuilder decoder_builder);

    VAE(const JSON& config);

    bool train(const unsigned num_epochs, const unsigned batch_size,
               const torch::Tensor& training_data,
               const std::optional<const torch::Tensor>& test_data = std::nullopt,
               const double weight_decay = 0., const bool trace = false,
               const unsigned test_every = 1e6) override;

    //Returns a mean and log var
    std::pair<torch::Tensor, torch::Tensor> encode(const torch::Tensor& x);
    torch::Tensor sample(const torch::Tensor& mu, const torch::Tensor& log_var,
                         const bool trace = false) const;

    //Returns output of VAE, mu and log_var from sampling
    std::tuple<torch::Tensor, torch::Tensor, torch::Tensor>
        vae_forward(const torch::Tensor& x, const bool trace = false);


private:

    torch::Tensor loss_function(const torch::Tensor& output,
                                const torch::Tensor& input,
                                const torch::Tensor& mu,
                                const torch::Tensor& log_var) const;

    std::optional<NetworkBuilder> create_encoder_builder(JSON config) const;
    NetworkBuilder create_decoder_builder(JSON config) const;

    std::unique_ptr<TorchNetwork> _encoder;

    torch::nn::Linear _encoder_mean_linear_layer;
    torch::nn::Linear _encoder_logvar_linear_layer;


};

} // namespace NeuroEvo

#endif
