#ifndef _LAYER_H_
#define _LAYER_H_

/*
    A layer consists of a vector of neurons.
*/

#include <phenotype/neural_network/neuron.h>
#include <vector>
#include <data/json.h>

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

    void set_trace(const bool trace);
    void set_weights(const std::vector<double>& weights);
    virtual void set_learning_rates(const std::vector<double>& learning_rates) {}

    unsigned get_number_of_weights() const;
    std::vector<double> get_weights() const;
    std::shared_ptr<ActivationFunction> get_activation_function() const;

    std::vector<double> evaluate(const std::vector<double>& inputs);

    void reset();

    void print(std::ostream& os) const;
    void print_weights_to_file(std::ofstream& file) const;
    void print_outputs_to_file(std::ofstream& file) const;

    JSON to_json() const;

protected:

    bool _trace;
    const unsigned _inputs_per_neuron;
    const unsigned _params_per_neuron;
    const unsigned _num_neurons;
    const NeuronType _neuron_type;
    const std::shared_ptr<ActivationFunction> _activation_function;
    const bool _bias;

    std::vector<std::unique_ptr<Neuron>> _neurons;

    void print_outputs(const std::vector<double>& outputs);

private:

    virtual Layer* clone_impl() const;

};

} // namespace NeuroEvo

#endif
