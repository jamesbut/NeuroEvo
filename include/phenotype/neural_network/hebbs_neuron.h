#ifndef _HEBBS_NEURON_H_
#define _HEBBS_NEURON_H_

#include <phenotype/phenotype_specs/layer_spec.h>
#include <phenotype/neural_network/neuron.h>
#include <vector>

namespace NeuroEvo {

class HebbsNeuron : public Neuron {

public:

    HebbsNeuron(const LayerSpec& layer_spec, const bool trace = false);

    void set_weights(const std::vector<double>& weights) override;
    void set_learning_rates(const std::vector<double>& learning_rates);

    double evaluate(const std::vector<double>& inputs) override;

    void reset() override;

private:

    std::vector<double> _learning_rates;

    //Determines how to change the synaptic weights after activation of the neuron
    void synaptic_weight_change(const std::vector<double>& inputs, const double output);

    void normalise_weights();

};

} // namespace NeuroEvo

#endif
