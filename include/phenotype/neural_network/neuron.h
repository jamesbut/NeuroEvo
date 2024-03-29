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
           const NeuronType& neuron_type,
           const std::shared_ptr<ActivationFunction> activation_function,
           const bool bias,
           const bool trace);

    virtual ~Neuron() = default;

    virtual void set_weights(const std::vector<double>& weights);
    virtual void set_learning_rates(const std::vector<double>& learning_rates) {}
    void set_trace(const bool trace);

    const std::vector<double>& get_weights() const;

    virtual double evaluate(const std::vector<double>& inputs);

    virtual void reset();
    auto clone() const
    {
        return std::unique_ptr<Neuron>(clone_impl());
    };

    void print(std::ostream& os) const;
    void print_weights_to_file(std::ofstream& file) const;
    void print_output_to_file(std::ofstream& file) const;

protected:

    virtual Neuron* clone_impl() const
    {
        return new Neuron(*this);
    };

    double propogate(const std::vector<double>& inputs);
    void check_num_weights(const std::vector<double>& weights) const;

    const unsigned _num_inputs;
    const NeuronType _neuron_type;
    const std::shared_ptr<ActivationFunction> _activation_function;
    const bool _bias;

    bool _trace;

    double _previous_output;
    std::vector<double> _weights;

};

} // namespace NeuroEvo

#endif
