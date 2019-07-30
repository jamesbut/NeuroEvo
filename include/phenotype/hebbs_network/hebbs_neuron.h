#ifndef _HEBBS_NEURON_H_
#define _HEBBS_NEURON_H_

#include <phenotype/phenotype_specs/layer_spec.h>
#include <vector>

namespace NeuroEvo {
namespace Phenotypes {

class HebbsNeuron {

public:

    HebbsNeuron(const LayerSpec& LAYER_SPEC, const bool TRACE);

    void set_weights(std::vector<double>& weights);
    void set_learning_rates(std::vector<double>& learning_rates);

    double evaluate(std::vector<double>& inputs);

    void print_weights();
    void print_weights_to_file(std::ofstream& file);
    void print_output_to_file(std::ofstream& file);

    void reset();

private:

    const LayerSpec _LAYER_SPEC;

    const bool _TRACE;

    double _previous_output;

    std::vector<double> _learning_rates;
    std::vector<double> _weights;

    //Determines how to change the synaptic weights after activation of the neuron
    void synaptic_weight_change(const std::vector<double>& inputs, const double output);

    void normalise_weights();

};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
