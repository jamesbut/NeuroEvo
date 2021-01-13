#ifndef _GAN_H_
#define _GAN_H_

/*
 * This class defines a Generative Adversarial Network using the PyTorch C++ API
 */

#include <util/maths/matrix.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <util/statistics/distributions/distribution.h>
#include <util/models/trainable_model.h>
#include <phenotype/neural_network/torch_network.h>

namespace NeuroEvo {

class GAN : public TrainableModel
{

public:

    GAN(NetworkBuilder& generator_builder,
        NetworkBuilder& discriminator_builder, 
        const torch::Tensor& real_data,
        Distribution<double>* init_net_weight_distr = nullptr);

    void train(const unsigned num_epochs, const unsigned batch_size, 
               const double weight_decay = 0., const bool trace = true, 
               const unsigned test_every = 1e6) override;

    torch::Tensor discriminate(const torch::Tensor& x) const;

private:

    double test_generator_symmetry(const bool random_noise) const;

    std::unique_ptr<TorchNetwork> _discriminator;

};

} // namespace NeuroEvo

#endif
