#ifndef _GAN_H_
#define _GAN_H_

/*
 * This class defines a Generative Adversarial Network using the PyTorch C++ API
 */

#include <util/maths/matrix.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <util/random/distribution.h>

namespace NeuroEvo {

class GAN
{

public:

    GAN(NetworkBuilder& generator_builder,
        NetworkBuilder& discriminator_builder, 
        const torch::Tensor& real_data,
        std::unique_ptr<Distribution<double>> init_net_weight_distr = 
            std::unique_ptr<Distribution<double>>(nullptr));

    void train(const unsigned num_epochs, const unsigned batch_size, const bool trace = true);

    torch::Tensor test_discriminator(const torch::Tensor& x) const;
    torch::Tensor test_generator(const torch::Tensor& x) const;

    const std::unique_ptr<TorchNetwork>& get_generator() const;

private:

    const torch::Tensor _real_data;

    std::unique_ptr<TorchNetwork> _generator;
    std::unique_ptr<TorchNetwork> _discriminator;

};

} // namespace NeuroEvo

#endif
