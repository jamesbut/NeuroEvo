#ifndef _AUTOENCODER_H_
#define _AUTOENCODER_H_

/*
 * This class defines an autoencoder using the PyTorch C++ API
 */

#include <phenotype/phenotype_specs/network_builder.h>

namespace NeuroEvo {

class AutoEncoder
{

public:

    AutoEncoder(const torch::Tensor& training_data, 
                const torch::Tensor& test_data, 
                NetworkBuilder& encoder_builder,
                NetworkBuilder& decoder_builder, 
                std::unique_ptr<Distribution<double>> init_net_weight_distr = 
                    std::unique_ptr<Distribution<double>>(nullptr));

    void train(const unsigned num_epochs, const unsigned batch_size, 
               const unsigned test_every = 1e6);

    torch::Tensor encode(const torch::Tensor& x) const;
    torch::Tensor decode(const torch::Tensor& x) const;
    torch::Tensor forward(const torch::Tensor& x) const;

private:

    const torch::Tensor _training_data;
    const torch::Tensor _test_data;

    std::unique_ptr<TorchNetwork> _encoder;
    std::unique_ptr<TorchNetwork> _decoder;

    torch::nn::Sequential _autoencoder;

};

} // namespace NeuroEvo

#endif
