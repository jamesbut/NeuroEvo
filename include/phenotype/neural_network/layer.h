#ifndef _LAYER_H_
#define _LAYER_H_

/*
    A layer consists of a vector of neurons.
*/

#include <phenotype/neural_network/neuron.h>
#include <vector>

namespace NeuroEvo {

class Layer {

public:

    Layer(const LayerSpec& layer_spec, const bool trace);
    Layer(const Layer& layer);

    virtual ~Layer() = default;

    auto clone() const
    {
        return std::unique_ptr<Layer>(clone_impl());
    }

    virtual void create_layer();

    void set_weights(const std::vector<double>& weights);
    virtual void set_learning_rates(const std::vector<double>& learning_rates) {}
    unsigned get_number_of_weights() const;
    std::vector<double> evaluate(const std::vector<double>& inputs);

    void reset();

    void print_weights() const;
    void print_params() const;
    void print_weights_to_file(std::ofstream& file) const;
    void print_outputs_to_file(std::ofstream& file) const;

protected:

    virtual Layer* clone_impl() const
    {
        return new Layer(*this);
    }

    const bool _trace;
    const unsigned _inputs_per_neuron;
    const unsigned _params_per_neuron;
    const unsigned _num_neurons;
    const NeuronType _neuron_type;
    const std::shared_ptr<ActivationFunction> _activation_function;

    std::vector<std::unique_ptr<Neuron> > _neurons;

    void print_outputs(const std::vector<double>& outputs);

};

} // namespace NeuroEvo

#endif
