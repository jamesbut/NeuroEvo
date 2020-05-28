#ifndef _NEURON_H_
#define _NEURON_H_

/*
    A neuron holds the connection weights of the connections
    that enter it.
*/

#include <phenotype/phenotype_specs/layer_spec.h>
#include <vector>

namespace NeuroEvo {

class Neuron {

public:

    Neuron(const unsigned num_inputs, 
           const LayerSpec::NeuronType& neuron_type, 
           const std::shared_ptr<ActivationFunction> activation_function,
           const bool trace);

    virtual void set_weights(const std::vector<double>& weights);
    virtual void set_learning_rates(const std::vector<double>& learning_rates) {}
    virtual double evaluate(const std::vector<double>& inputs);

    virtual void reset();
    auto clone() const 
    { 
        return std::unique_ptr<Neuron>(clone_impl()); 
    };

    void print_weights() const;
    void print_weights_to_file(std::ofstream& file) const;
    void print_output_to_file(std::ofstream& file) const;

protected:

    virtual Neuron* clone_impl() const 
    { 
        return new Neuron(*this); 
    };

    double propogate(const std::vector<double>& inputs);

    const unsigned _num_inputs;
    const LayerSpec::NeuronType _neuron_type;
    const std::shared_ptr<ActivationFunction> _activation_function;

    const bool _trace;

    double _previous_output;
    std::vector<double> _weights;

};

} // namespace NeuroEvo

#endif
