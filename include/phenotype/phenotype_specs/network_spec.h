#ifndef _NETWORK_SPEC_H_
#define _NETWORK_SPEC_H_

/*
    Using this Network Spec class helps group together
    different types of networks and makes the code cleaner.
*/

#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <phenotype/phenotype_specs/layer_spec.h>

namespace NeuroEvo {
namespace Phenotypes {

struct NetworkSpec : PhenotypeSpec {

public:

    NetworkSpec(const unsigned NUM_INPUTS, const unsigned NUM_OUTPUTS,
                const std::vector<LayerSpec>& LAYER_SPECS, const unsigned NUM_GENES) :
        NUM_INPUTS(NUM_INPUTS),
        NUM_OUTPUTS(NUM_OUTPUTS),
        LAYER_SPECS(LAYER_SPECS),
        PhenotypeSpec(NUM_GENES) {}


    virtual Genotypes::Genotype* generate_genotype() = 0;
    virtual Genotypes::Genotype* generate_genotype(Genotypes::Genotype& genotype) = 0;
    virtual Genotypes::Genotype* generate_genotype(const std::string& file_name) = 0;
    virtual Phenotypes::Phenotype* generate_phenotype(Genotypes::Genotype& genotype,
                                                      GPMaps::GPMap* gp_map) = 0;

    auto clone() const { return std::unique_ptr<NetworkSpec>(clone_impl()); }

    const unsigned int NUM_INPUTS;
    const unsigned int NUM_OUTPUTS;

    const std::vector<LayerSpec> LAYER_SPECS;

protected:

    virtual NetworkSpec* clone_impl() const = 0;

    const unsigned get_num_outputs(const std::vector<LayerSpec>& layer_specs) const {
        return layer_specs.back()._num_neurons;
    }

};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
