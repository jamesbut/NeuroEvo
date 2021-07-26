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

//Data and parameters when using a conditional GAN
//One has to give the conditional labels of the training data and some way
//of randomly generating random conditional values in the generators latent space
struct cGANParams
{
    //The conditional labels for the training data
    torch::Tensor conditional_labels;

    //The lower and upper bounds used to generate random conditional data
    double conditional_lb;
    double conditional_ub;
};


class GAN : public TrainableModel
{

public:

    GAN(NetworkBuilder& generator_builder,
        NetworkBuilder& discriminator_builder,
        const torch::Tensor& real_data,
        const std::optional<cGANParams>& cgan_params);

    bool train(const unsigned num_epochs, const unsigned batch_size,
               const double weight_decay = 0., const bool trace = true,
               const unsigned test_every = 1e6) override;

    torch::Tensor discriminate(const torch::Tensor& x) const;

private:

    double test_generator_symmetry(const bool random_noise) const;

    torch::Tensor draw_conditional_noise(const unsigned num_noise_vecs) const;

    std::unique_ptr<TorchNetwork> _discriminator;

    const std::optional<cGANParams> _cgan_params;

};

} // namespace NeuroEvo

#endif
