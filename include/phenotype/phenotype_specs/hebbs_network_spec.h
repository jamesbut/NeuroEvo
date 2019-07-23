#ifndef _HEBBS_NETWORK_SPEC_H_
#define _HEBBS_NETWORK_SPEC_H_

#include <phenotype/phenotype_specs/network_spec.h>

struct HebbsNetworkSpec : NetworkSpec {

public:

    HebbsNetworkSpec(const unsigned NUM_INPUTS, const unsigned NUM_OUTPUTS,
                     const unsigned NUM_HIDDEN_LAYERS, const unsigned NEURONS_PER_LAYER,
                     const bool RECURRENT, const bool RANDOM_WEIGHT_INIT, const bool EVOLVE_INIT_WEIGHTS,
                     const bool TRACE = false);

    HebbsNetworkSpec(const unsigned NUM_INPUTS, const std::vector<LayerSpec>& layer_specs,
                     const bool RANDOM_WEIGHT_INIT, const bool EVOLVE_INIT_WEIGHTS, const bool TRACE = false);


    Genotype* generate_genotype() override;
    Genotype* generate_genotype(Genotype& genotype) override;
    Genotype* generate_genotype(const std::string& file_name) override;
    GPMap* generate_gp_map() override;
    GPMap* generate_gp_map(const std::string& file_name) override;
    Phenotype* generate_phenotype(Genotype& genotype, GPMap* gp_map) override;

    const bool RANDOM_WEIGHT_INIT;
    const bool EVOLVE_INIT_WEIGHTS;

protected:

    virtual HebbsNetworkSpec* clone_impl() const override { return new HebbsNetworkSpec(*this); };

    const unsigned get_required_num_genes(const unsigned int NUM_INPUTS,
                                          const unsigned int NUM_OUPUTS,
                                          const unsigned int NUM_HIDDEN_LAYERS,
                                          const unsigned int NEURONS_PER_LAYER,
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

#endif
