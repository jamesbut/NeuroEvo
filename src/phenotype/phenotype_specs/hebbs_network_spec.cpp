#include <phenotype/phenotype_specs/hebbs_network_spec.h>
#include <genotype/real_vector_genotype.h>
#include <phenotype/hebbs_network/hebbs_network.h>
#include <iostream>

HebbsNetworkSpec::HebbsNetworkSpec(const unsigned int NUM_INPUTS, const unsigned int NUM_OUTPUTS,
                                   const unsigned int NUM_HIDDEN_LAYERS, const unsigned int NEURONS_PER_LAYER,
                                   const bool RECURRENT, const bool RANDOM_WEIGHT_INIT,
                                   const bool EVOLVE_INIT_WEIGHTS, const bool TRACE) :
    RANDOM_WEIGHT_INIT(RANDOM_WEIGHT_INIT),
    EVOLVE_INIT_WEIGHTS(EVOLVE_INIT_WEIGHTS),
    NetworkSpec(NUM_INPUTS, NUM_OUTPUTS,
                build_layer_specs(NUM_INPUTS, NUM_OUTPUTS, NUM_HIDDEN_LAYERS, NEURONS_PER_LAYER, RECURRENT),
                get_required_num_genes(NUM_INPUTS,
                                       NUM_OUTPUTS,
                                       NUM_HIDDEN_LAYERS,
                                       NEURONS_PER_LAYER,
                                       RECURRENT,
                                       EVOLVE_INIT_WEIGHTS),
                TRACE) {}


HebbsNetworkSpec::HebbsNetworkSpec(const unsigned NUM_INPUTS, const std::vector<LayerSpec>& layer_specs,
                                   const bool RANDOM_WEIGHT_INIT, const bool EVOLVE_INIT_WEIGHTS,
                                   const bool TRACE) :
    RANDOM_WEIGHT_INIT(RANDOM_WEIGHT_INIT),
    EVOLVE_INIT_WEIGHTS(EVOLVE_INIT_WEIGHTS),
    NetworkSpec(NUM_INPUTS, get_num_outputs(layer_specs), layer_specs,
                get_required_num_genes(NUM_INPUTS, layer_specs, EVOLVE_INIT_WEIGHTS), TRACE) {}

const unsigned HebbsNetworkSpec::get_required_num_genes(const unsigned int NUM_INPUTS,
                                                        const unsigned int NUM_OUTPUTS,
                                                        const unsigned int NUM_HIDDEN_LAYERS,
                                                        const unsigned int NEURONS_PER_LAYER,
                                                        const bool RECURRENT,
                                                        const bool EVOLVE_INIT_WEIGHTS) {

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

    if(EVOLVE_INIT_WEIGHTS) return numWeights * 2;
    else return numWeights;

}

const unsigned HebbsNetworkSpec::get_required_num_genes(const unsigned NUM_INPUTS,
                                                        const std::vector<LayerSpec>& layer_specs,
                                                        const bool EVOLVE_INIT_WEIGHTS) {

    unsigned num_weights = 0;

    for(const auto& layer_spec : layer_specs)
        num_weights += layer_spec._num_neurons * layer_spec._inputs_per_neuron;

    if(EVOLVE_INIT_WEIGHTS) return num_weights * 2;
    else return num_weights;

}

std::vector<LayerSpec> HebbsNetworkSpec::build_layer_specs(const unsigned NUM_INPUTS,
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

Genotype* HebbsNetworkSpec::generate_genotype() {

    return new RealVectorGenotype(NUM_GENES);

}

Genotype* HebbsNetworkSpec::generate_genotype(Genotype& genotype) {

    RealVectorGenotype* real_vec_genotype = dynamic_cast<RealVectorGenotype*>(&genotype);

    return new RealVectorGenotype(*real_vec_genotype);

}

Genotype* HebbsNetworkSpec::generate_genotype(const std::string& file_name) {

    return new RealVectorGenotype(file_name);

}

GPMap* HebbsNetworkSpec::generate_gp_map() {

    return nullptr;

}

GPMap* HebbsNetworkSpec::generate_gp_map(const std::string& file_name) {

    return nullptr;

}

Phenotype* HebbsNetworkSpec::generate_phenotype(Genotype& genotype, GPMap* gp_map) {

    RealVectorGenotype* real_vec_genotype = dynamic_cast<RealVectorGenotype*>(&genotype);

    return new HebbsNetwork(*real_vec_genotype, *this, TRACE);

}
