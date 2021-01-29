#ifndef _AUTOENCODER_H_
#define _AUTOENCODER_H_

/*
 * This class defines an autoencoder using the PyTorch C++ API
 */

#include <phenotype/phenotype_specs/network_builder.h>
#include <util/models/trainable_model.h>

namespace NeuroEvo {

class AutoEncoder : public TrainableModel
{

public:

    AutoEncoder(NetworkBuilder& encoder_builder,
                NetworkBuilder& decoder_builder, 
                const torch::Tensor& training_data, 
                const std::optional<const torch::Tensor>& test_data = std::nullopt);

    void train(const unsigned num_epochs, const unsigned batch_size, 
               const double weight_decay = 0., const bool trace = false, 
               const unsigned test_every = 1e6) override;

    torch::Tensor encode(const torch::Tensor& x) const;
    std::tuple<torch::Tensor, torch::Tensor> ae_forward(const torch::Tensor& x);

private:

    torch::Tensor loss_function(const torch::Tensor& output, const torch::Tensor& input) const;

    std::unique_ptr<TorchNetwork> _encoder;

    torch::nn::Sequential _autoencoder;

};

} // namespace NeuroEvo

#endif
