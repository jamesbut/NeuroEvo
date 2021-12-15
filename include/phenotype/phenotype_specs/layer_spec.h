#ifndef _LAYER_SPEC_H_
#define _LAYER_SPEC_H_

#include <math.h>
#include <memory>
#include <iostream>
#include <util/maths/activation_functions/activation_function.h>
#include <vector>
#include <util/maths/activation_functions/activation_function_specs/activation_function_spec.h>

/*
    Defines an activation function description
    and implements that activation function.
*/

namespace NeuroEvo {

/*
    Defines a network layer.
    Only normal, recurrent and GRU units can be used currently.
*/

enum NeuronType
{
    Standard,
    Recurrent,
    GRU
};

class LayerSpec
{

public:

    LayerSpec(const unsigned num_neurons,
              const unsigned inputs_per_neuron,
              const std::shared_ptr<ActivationFunctionSpec> activation_func_spec,
              const bool batch_norm = false,
              const NeuronType neuron_type = NeuronType::Standard,
              const bool bias = true);

    LayerSpec(const LayerSpec& layer_spec);
    LayerSpec& operator=(const LayerSpec& layer_spec);
    LayerSpec(LayerSpec&& layer_spec) = default;
    LayerSpec& operator=(LayerSpec&& layer_spec) = default;
    ~LayerSpec() = default;

    //A static function to build a number of layers according to a specification
    //This is helpful
    static std::vector<LayerSpec> build_layer_specs(
            const unsigned num_inputs,
            const unsigned num_outputs,
            const unsigned num_hidden_layers,
            const unsigned neurons_per_layer,
            const NeuronType neuron_type,
            const std::shared_ptr<ActivationFunctionSpec>& activation_func_spec,
            const bool batch_norm = false,
            const bool bias = true);

    static std::vector<LayerSpec> build_layer_specs(
            const unsigned num_inputs,
            const unsigned num_outputs,
            const unsigned num_hidden_layers,
            const unsigned neurons_per_layer,
            const NeuronType neuron_type,
            const std::shared_ptr<ActivationFunctionSpec>& hl_activation_func_spec,
            const std::shared_ptr<ActivationFunctionSpec>& ol_activation_func_spec,
            const bool batch_norm = false,
            const bool bias = true);

    NeuronType get_neuron_type() const;
    unsigned get_num_neurons() const;
    unsigned get_inputs_per_neuron() const;
    unsigned get_params_per_neuron() const;
    unsigned get_num_weights() const;
    const std::shared_ptr<ActivationFunctionSpec>& get_activation_func_spec() const;
    bool get_batch_norm() const;
    bool get_bias() const;

    void print_spec() const;

    JSON to_json() const;

private:

    NeuronType _neuron_type;
    unsigned _num_neurons;
    unsigned _inputs_per_neuron;
    unsigned _params_per_neuron;

    std::shared_ptr<ActivationFunctionSpec> _activation_func_spec;

    bool _batch_norm;

    bool _bias;

};

} // namespace NeuroEvo

#endif
