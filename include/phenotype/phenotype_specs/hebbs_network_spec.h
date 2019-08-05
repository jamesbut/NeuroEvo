#ifndef _HEBBS_NETWORK_SPEC_H_
#define _HEBBS_NETWORK_SPEC_H_

#include <phenotype/phenotype_specs/network_spec.h>

namespace NeuroEvo {
namespace Phenotypes {

struct HebbsNetworkSpec : NetworkSpec {

public:

    HebbsNetworkSpec(const unsigned NUM_INPUTS, const unsigned NUM_OUTPUTS,
                     const unsigned NUM_HIDDEN_LAYERS, const unsigned NEURONS_PER_LAYER,
                     const bool RECURRENT, const bool RANDOM_WEIGHT_INIT, const bool EVOLVE_INIT_WEIGHTS,
                     const GPMaps::gpmaps GPMAP_IDENTIFER = GPMaps::NoMap);

    HebbsNetworkSpec(const unsigned NUM_INPUTS, const std::vector<LayerSpec>& layer_specs,
                     const bool RANDOM_WEIGHT_INIT, const bool EVOLVE_INIT_WEIGHTS,
                     const GPMaps::gpmaps GPMAP_IDENTIFIER = GPMaps::NoMap);


    Genotypes::Genotype* generate_genotype() override;
    Genotypes::Genotype* generate_genotype(Genotypes::Genotype& genotype) override;
    Genotypes::Genotype* generate_genotype(const std::string& file_name) override;
    GPMaps::GPMap* generate_gp_map() override;
    GPMaps::GPMap* generate_gp_map(const std::string& file_name) override;
    Phenotypes::Phenotype* generate_phenotype(Genotypes::Genotype& genotype,
                                              GPMaps::GPMap* gp_map) override;

    const bool RANDOM_WEIGHT_INIT;
    const bool EVOLVE_INIT_WEIGHTS;

protected:

    virtual HebbsNetworkSpec* clone_impl() const override { return new HebbsNetworkSpec(*this); };

    const unsigned get_required_num_genes(const unsigned NUM_INPUTS,
                                          const unsigned NUM_OUPUTS,
                                          const unsigned NUM_HIDDEN_LAYERS,
                                          const unsigned NEURONS_PER_LAYER,
                                          const bool RECURRENT,
                                          const bool EVOLVE_INIT_WEIGHTS);

    const unsigned get_required_num_genes(const unsigned NUM_INPUTS,
                                          const std::vector<LayerSpec>& layer_specs,
                                          const bool EVOLVE_INIT_WEIGHTS);

private:

    std::vector<LayerSpec> build_layer_specs(const unsigned NUM_INPUTS,
                                             const unsigned NUM_OUTPUTS,
                                             const unsigned NUM_HIDDEN_LAYERS,
                                             const unsigned NEURONS_PER_LAYER,
                                             const bool RECURRENT);
};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
