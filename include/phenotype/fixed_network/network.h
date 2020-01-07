#ifndef _NETWORK_H_
#define _NETWORK_H_

/*
    Defines a fixed network as a vector of layers.
*/

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <phenotype/fixed_network/layer.h>
#include <genotype/real_vector_genotype.h>

namespace NeuroEvo {
namespace Phenotypes {

class FixedNetwork : public Phenotype {

public:

    FixedNetwork(const FixedNetworkSpec& NET_SPEC);

    FixedNetwork(const std::vector<double>& traits, FixedNetworkSpec& NET_SPEC);

    void propogate_weights(const std::vector<double>& weights);
    std::vector<double> activate(std::vector<double>& inputs) override;

    void reset() override;

protected:

    virtual FixedNetwork* clone_impl() const override { return new FixedNetwork(*this); };

    void print_params() override {
        for(auto& layer : layers)
            layer.print_params();
    };

private:

    void create_net();

    const FixedNetworkSpec _NET_SPEC;

    const bool _TRACE;

    std::vector<Layer> layers;

};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
