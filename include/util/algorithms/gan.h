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

    GAN(const Matrix<double>& true_data, NetworkBuilder& generator_builder,
        NetworkBuilder& discriminator_builder, 
        std::unique_ptr<Distribution<double>> init_net_weight_distr = 
            std::unique_ptr<Distribution<double>>(nullptr));

private:

    TorchNetwork* build_network(NetworkBuilder& net_builder, 
                                std::unique_ptr<Distribution<double>>& init_net_weight_distr);

    const Matrix<double> _true_data;

    std::unique_ptr<TorchNetwork> _generator;
    std::unique_ptr<TorchNetwork> _discriminator;

};

} // namespace NeuroEvo

#endif
