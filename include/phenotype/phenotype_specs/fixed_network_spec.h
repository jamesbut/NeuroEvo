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

//Define fixed network specification
struct FixedNetworkSpec : NetworkSpec {

public:


    FixedNetworkSpec(const unsigned NUM_INPUTS, const unsigned NUM_OUTPUTS,
                     const unsigned NUM_HIDDEN_LAYERS, const unsigned NEURONS_PER_LAYER,
                     const bool RECURRENT, const bool TRACE = false);


    FixedNetworkSpec(const unsigned NUM_INPUTS, const std::vector<LayerSpec>& layer_specs,
                     const bool TRACE = false);


    Genotype* generate_genotype() override;
    Genotype* generate_genotype(Genotype& genotype) override;
    Genotype* generate_genotype(const std::string& file_name) override;
    GPMap* generate_gp_map() override;
    GPMap* generate_gp_map(const std::string& file_name) override;
    Phenotype* generate_phenotype(Genotype& genotype, GPMap* gp_map) override;

protected:

    virtual FixedNetworkSpec* clone_impl() const override { return new FixedNetworkSpec(*this); };

    const unsigned get_required_num_genes(const unsigned int NUM_INPUTS,
                                          const unsigned int NUM_OUPUTS,
                                          const unsigned int NUM_HIDDEN_LAYERS,
                                          const unsigned int NEURONS_PER_LAYER,
                                          const bool RECURRENT);

    const unsigned get_required_num_genes(const unsigned NUM_INPUTS,
                                          const std::vector<LayerSpec>& layer_specs);

private:

    std::vector<LayerSpec> build_layer_specs(const unsigned NUM_INPUTS,
                                             const unsigned NUM_OUTPUTS,
                                             const unsigned NUM_HIDDEN_LAYERS,
                                             const unsigned NEURONS_PER_LAYER,
                                             const bool RECURRENT);

};

#endif
