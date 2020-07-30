#ifndef _GAN_H_
#define _GAN_H_

/*
 * This class defines a Generative Adversarial Network using the PyTorch C++ API
 */

#include <util/maths/matrix.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <util/statistics/distributions/distribution.h>
#include <util/models/generative_models/generative_model.h>

namespace NeuroEvo {

class GAN : public GenerativeModel
{

public:

    GAN(NetworkBuilder& generator_builder,
        NetworkBuilder& discriminator_builder, 
        const torch::Tensor& real_data,
        std::unique_ptr<Distribution<double>> init_net_weight_distr = 
            std::unique_ptr<Distribution<double>>(nullptr));

    void train(const unsigned num_epochs, const unsigned batch_size, 
               const bool trace = true, const unsigned test_every = 1e6) override;

    torch::Tensor discriminate(const torch::Tensor& x) const;
    torch::Tensor generate(const torch::Tensor& x) const override;

    const std::unique_ptr<TorchNetwork>& get_generator() const;

private:

    std::unique_ptr<TorchNetwork> _generator;
    std::unique_ptr<TorchNetwork> _discriminator;

};

} // namespace NeuroEvo

#endif
