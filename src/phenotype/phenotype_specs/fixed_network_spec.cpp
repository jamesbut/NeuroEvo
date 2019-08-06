#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <genotype/real_vector_genotype.h>
#include <phenotype/fixed_network/network.h>
#include <iostream>
#include <gp_map/matrix_map.h>
#include <gp_map/dct_map.h>

namespace NeuroEvo {
namespace Phenotypes {

FixedNetworkSpec::FixedNetworkSpec(const unsigned NUM_INPUTS, const unsigned NUM_OUTPUTS,
                                   const unsigned NUM_HIDDEN_LAYERS, const unsigned NEURONS_PER_LAYER,
                                   const bool RECURRENT) :
    NetworkSpec(NUM_INPUTS, NUM_OUTPUTS,
                build_layer_specs(NUM_INPUTS, NUM_OUTPUTS,
                                  NUM_HIDDEN_LAYERS, NEURONS_PER_LAYER,
                                  RECURRENT),
                get_required_num_genes(NUM_INPUTS,
                                       NUM_OUTPUTS,
                                       NUM_HIDDEN_LAYERS,
                                       NEURONS_PER_LAYER,
                                       RECURRENT)) {}


FixedNetworkSpec::FixedNetworkSpec(const unsigned NUM_INPUTS, const std::vector<LayerSpec>& layer_specs) :
    NetworkSpec(NUM_INPUTS, get_num_outputs(layer_specs), layer_specs,
                get_required_num_genes(NUM_INPUTS, layer_specs)) {}


const unsigned FixedNetworkSpec::get_required_num_genes(const unsigned int NUM_INPUTS,
                                                        const unsigned int NUM_OUTPUTS,
                                                        const unsigned int NUM_HIDDEN_LAYERS,
                                                        const unsigned int NEURONS_PER_LAYER,
                                                        const bool RECURRENT) {

    int numWeights = 0;

    if (NUM_HIDDEN_LAYERS == 0) {

        numWeights += (NUM_INPUTS+1) * NUM_OUTPUTS;

        if(RECURRENT) numWeights += NUM_OUTPUTS;

    } else {

        numWeights += (NUM_INPUTS+1) * NEURONS_PER_LAYER;
        numWeights += (NEURONS_PER_LAYER+1) * NEURONS_PER_LAYER * (NUM_HIDDEN_LAYERS-1);
        numWeights += (NEURONS_PER_LAYER+1) * NUM_OUTPUTS;

        if(RECURRENT) numWeights += NEURONS_PER_LAYER * NUM_HIDDEN_LAYERS + NUM_OUTPUTS;

    }

    return numWeights;

}

const unsigned FixedNetworkSpec::get_required_num_genes(const unsigned NUM_INPUTS,
                                                        const std::vector<LayerSpec>& layer_specs) {

    unsigned num_weights = 0;

    for(const auto& layer_spec : layer_specs)
        num_weights += layer_spec._num_neurons * layer_spec._params_per_neuron;

    return num_weights;

}

std::vector<LayerSpec> FixedNetworkSpec::build_layer_specs(const unsigned NUM_INPUTS,
                                                           const unsigned NUM_OUTPUTS,
                                                           const unsigned NUM_HIDDEN_LAYERS,
                                                           const unsigned NEURONS_PER_LAYER,
                                                           const bool RECURRENT) {

    std::vector<LayerSpec> layer_specs;

    const unsigned neuron_type = RECURRENT ? 1 : 0;

    if(NUM_HIDDEN_LAYERS == 0)

        layer_specs.push_back(LayerSpec(neuron_type, NUM_OUTPUTS, NUM_INPUTS,
                                        new SigmoidSpec(1.0)));

    else {

        layer_specs.push_back(LayerSpec(neuron_type, NEURONS_PER_LAYER, NUM_INPUTS,
                                        new SigmoidSpec(1.0)));

        for(unsigned i = 1; i < NUM_HIDDEN_LAYERS; i++)
            layer_specs.push_back(LayerSpec(neuron_type, NEURONS_PER_LAYER, NEURONS_PER_LAYER,
                                            new SigmoidSpec(1.0)));

        layer_specs.push_back(LayerSpec(neuron_type, NUM_OUTPUTS, NEURONS_PER_LAYER,
                                        new SigmoidSpec(1.0)));

    }

    return layer_specs;

}


Genotypes::Genotype* FixedNetworkSpec::generate_genotype() {

    return new Genotypes::RealVectorGenotype(NUM_GENES);

}

Genotypes::Genotype* FixedNetworkSpec::generate_genotype(Genotypes::Genotype& genotype) {

    Genotypes::RealVectorGenotype* real_vec_genotype =
        dynamic_cast<Genotypes::RealVectorGenotype*>(&genotype);

    return new Genotypes::RealVectorGenotype(*real_vec_genotype);

}

Genotypes::Genotype* FixedNetworkSpec::generate_genotype(const std::string& file_name) {

    return new Genotypes::RealVectorGenotype(file_name);

}

Phenotypes::Phenotype* FixedNetworkSpec::generate_phenotype(Genotypes::Genotype& genotype,
                                                            GPMaps::GPMap* gp_map) {

    Genotypes::RealVectorGenotype* real_vec_genotype =
        dynamic_cast<Genotypes::RealVectorGenotype*>(&genotype);

    //If there is a gp_map, then map, otherwise
    //just use the genes for traits
    if(gp_map)
        return gp_map->map(genotype, *this);
    else
        return new Phenotypes::FixedNetwork(real_vec_genotype->get_genes(), *this);

}

} // namespace Phenotypes
} // namespace NeuroEvo
