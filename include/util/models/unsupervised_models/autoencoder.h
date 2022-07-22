#ifndef _AUTOENCODER_H_
#define _AUTOENCODER_H_

/*
 * This class defines an autoencoder using the PyTorch C++ API
 * It also provides functionality for a denoising autoencoder too
 */

#include <phenotype/phenotype_specs/network_builder.h>
#include <util/models/trainable_model.h>

namespace NeuroEvo {

class AutoEncoder : public TrainableModel
{

public:

    AutoEncoder(NetworkBuilder encoder_builder,
                NetworkBuilder decoder_builder,
                const double learning_rate,
                const bool tied_weights = false,
                //This is the standard deviation of the gaussian of a denoising AE's
                //corruption function. If this value is not a null optional, this
                //AE now acts as a denoising AE.
                const std::optional<const double> denoising_sigma = std::nullopt);

    AutoEncoder(const JSON& config);

    bool train(const unsigned num_epochs, const unsigned batch_size,
               const torch::Tensor& training_data,
               const std::optional<const torch::Tensor>& test_data = std::nullopt,
               const double weight_decay = 0., const bool trace = false,
               const unsigned test_every = 1e6) override;

    torch::Tensor encode(const torch::Tensor& x) const;
    std::tuple<torch::Tensor, torch::Tensor> ae_forward(const torch::Tensor& x);

private:

    torch::Tensor loss_function(const torch::Tensor& output,
                                const torch::Tensor& input) const;

    //Ties the weights of the encoder and decoder to be the transposition of one another
    void tie_weights();

    //Create encoder and decoder builders from json
    NetworkBuilder create_encoder_builder(JSON config) const;
    NetworkBuilder create_decoder_builder(JSON config) const;

    std::unique_ptr<TorchNetwork> _encoder;
    torch::nn::Sequential _autoencoder;

    const double _learning_rate;

    const std::optional<const double> _denoising_sigma;
    const bool _tied_weights;

};

} // namespace NeuroEvo

#endif
