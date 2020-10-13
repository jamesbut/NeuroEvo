#ifndef _VAE_H_
#define _VAE_H_

/*
 * This class defines a Variational Autoencoder using the Torch C++ API
 */

#include <phenotype/phenotype_specs/network_builder.h>
#include <util/models/generative_models/generative_model.h>
#include <phenotype/neural_network/torch_network.h>

namespace NeuroEvo {

class VAE : public GenerativeModel
{

public:

    VAE(NetworkBuilder* encoder_builder,
        NetworkBuilder& decoder_builder,
        const torch::Tensor& training_data,
        const std::optional<const torch::Tensor>& test_data = std::nullopt,
        Distribution<double>* init_net_weight_distr = nullptr);

    virtual void train(const unsigned num_epochs, const unsigned batch_size,
                       const double weight_decay = 0., const bool trace = false, 
                       const unsigned test_every = 1e6) override;

    //Returns a mean and log var
    std::pair<torch::Tensor, torch::Tensor> encode(const torch::Tensor& x);
    torch::Tensor sample(const torch::Tensor& mu, const torch::Tensor& log_var,
                         const bool trace = false) const;
    torch::Tensor generate(const torch::Tensor& z) const override;

    //Returns output of VAE, mu and log_var from sampling
    std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> forward(const torch::Tensor& x,
                                                                    const bool trace = false);

    const std::unique_ptr<TorchNetwork>& get_decoder() const; 

private:

    torch::Tensor loss_function(const torch::Tensor& output, const torch::Tensor& input,
                                const torch::Tensor& mu, const torch::Tensor& log_var) const;

    std::unique_ptr<TorchNetwork> _encoder;
    std::unique_ptr<TorchNetwork> _decoder;

    torch::nn::Linear _encoder_mean_linear_layer;
    torch::nn::Linear _encoder_logvar_linear_layer;


};

} // namespace NeuroEvo

#endif
