#ifndef _FIXED_NETWORK_SPEC_H_
#define _FIXED_NETWORK_SPEC_H_

/*
    Defines how to build a fixed network given a number of
    parameters.
    One can give a simple description: num_inputs, num_outputs,
    num_hidden_layers and neurons_per_layer OR
    One can give a more fine grained example specifying each
    specific layer.
*/

#include <phenotype/phenotype_specs/network_spec.h>

namespace NeuroEvo {

//Define fixed network specification
template <typename G>
class FixedNetworkSpec : NetworkSpec<G> {

public:

    FixedNetworkSpec(const unsigned num_inputs, const unsigned num_outputs,
                     const unsigned num_hidden_layers, const unsigned neurons_per_layer,
                     const bool recurrent);

    FixedNetworkSpec(const unsigned num_inputs, const std::vector<LayerSpec>& layer_specs);

    Phenotype* generate_phenotype(Genotype<G>& genotype,
                                  GPMap<G>* gp_map) override;

protected:

    virtual FixedNetworkSpec* clone_impl() const override { return new FixedNetworkSpec(*this); };

    const unsigned get_required_num_genes(const unsigned int num_inputs,
                                          const unsigned int num_ouputs,
                                          const unsigned int num_hidden_layers,
                                          const unsigned int neurons_per_layer,
                                          const bool recurrent);

    const unsigned get_required_num_genes(const unsigned num_inputs,
                                          const std::vector<LayerSpec>& layer_specs);

private:

    std::vector<LayerSpec> build_layer_specs(const unsigned num_inputs,
                                             const unsigned num_outputs,
                                             const unsigned num_hidden_layers,
                                             const unsigned neurons_per_layer,
                                             const bool recurrent);

};

} // namespace NeuroEvo

#endif
