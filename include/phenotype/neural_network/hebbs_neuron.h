#ifndef _HEBBS_NEURON_H_
#define _HEBBS_NEURON_H_

#include <algorithm>
#include <phenotype/phenotype_specs/layer_spec.h>
#include <phenotype/neural_network/neuron.h>
#include <vector>

namespace NeuroEvo {

class HebbsNeuron : public Neuron {

public:

    HebbsNeuron(const unsigned num_input, 
                const NeuronType& neuron_type, 
                const std::shared_ptr<ActivationFunction> activation_function,
                const bool trace = false);

    void set_weights(const std::vector<double>& weights) override;
    void set_learning_rates(const std::vector<double>& learning_rates) override;

    double evaluate(const std::vector<double>& inputs) override;

    void reset() override;

protected:

    Neuron* clone_impl() const override
    {
        return new HebbsNeuron(*this);
    }


private:

    std::vector<double> _learning_rates;

    //Determines how to change the synaptic weights after activation of the neuron
    void synaptic_weight_change(const std::vector<double>& inputs, const double output);

    void normalise_weights();

};

} // namespace NeuroEvo

#endif
