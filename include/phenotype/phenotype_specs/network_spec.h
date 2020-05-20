#ifndef _NETWORK_SPEC_H_
#define _NETWORK_SPEC_H_

/*
    Using this Network Spec class helps group together
    different types of networks and makes the code cleaner.
*/

#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <phenotype/phenotype_specs/layer_spec.h>

namespace NeuroEvo {

template <typename G>
class NetworkSpec : PhenotypeSpec<G> {

public:

    virtual ~NetworkSpec() = default;

    NetworkSpec(const unsigned num_inputs, const unsigned num_outputs,
                const std::vector<LayerSpec>& layer_specs, const unsigned num_genes) :
        _num_inputs(num_inputs),
        _num_outputs(num_outputs),
        _layer_specs(layer_specs),
        PhenotypeSpec<G>(num_genes) {}

    virtual Phenotype* generate_phenotype(Genotype<G>& genotype,
                                          GPMap<G>* gp_map) = 0;

    auto clone() const { return std::unique_ptr<NetworkSpec>(clone_impl()); }

    const unsigned int _num_inputs;
    const unsigned int _num_outputs;

    const std::vector<LayerSpec> _layer_specs;

protected:

    virtual NetworkSpec* clone_impl() const = 0;

    const unsigned get_num_outputs(const std::vector<LayerSpec>& layer_specs) const {
        return layer_specs.back()._num_neurons;
    }

};

} // namespace NeuroEvo

#endif
