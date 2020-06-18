#ifndef _LAYER_SPEC_H_
#define _LAYER_SPEC_H_

#include <math.h>
#include <memory>
#include <iostream>
#include <util/maths/activation_functions/activation_function.h>
#include <vector>

//TODO: REMOVE
#include <util/maths/activation_functions/activation_function_specs/sigmoid_spec.h>

/*
    Defines an activation function description
    and implements that activation function.
*/

namespace NeuroEvo {

/*
    Defines a network layer.
    Only normal, recurrent and GRU units can be used currently.
*/

class LayerSpec 
{

public:

    enum NeuronType 
    {
        Standard,
        Recurrent,
        GRU
    };

    LayerSpec(const NeuronType neuron_type, const unsigned num_neurons,
              const unsigned inputs_per_neuron, 
              ActivationFunction* activation_func) :
        _neuron_type(neuron_type),
        _num_neurons(num_neurons),
        _inputs_per_neuron(inputs_per_neuron),
        _activation_func(activation_func)
    {

        //Normal
        if(_neuron_type == Standard)
            _params_per_neuron = inputs_per_neuron + 1;

        //Recurrent
        if(_neuron_type == Recurrent)
            _params_per_neuron = inputs_per_neuron + 2;

        //GRU
        if(_neuron_type == GRU) 
            _params_per_neuron = (inputs_per_neuron * 3) + 6;

    }

    LayerSpec(const LayerSpec& layer_spec) :
        _neuron_type(layer_spec._neuron_type),
        _num_neurons(layer_spec._num_neurons),
        _inputs_per_neuron(layer_spec._inputs_per_neuron),
        _params_per_neuron(layer_spec._params_per_neuron),
        _activation_func(layer_spec._activation_func) {}

    LayerSpec& operator=(const LayerSpec& layer_spec) 
    {

        _neuron_type = layer_spec._neuron_type;
        _num_neurons = layer_spec._num_neurons;
        _inputs_per_neuron = layer_spec._inputs_per_neuron;
        _params_per_neuron = layer_spec._params_per_neuron;
        _activation_func = layer_spec._activation_func;

        return *this;

    }

    //A static function to build a number of layers according to a specification
    //This is helpful
    static std::vector<LayerSpec> build_layer_specs(const unsigned num_inputs,
                                                    const unsigned num_outputs,
                                                    const unsigned num_hidden_layers,
                                                    const unsigned neurons_per_layer,
                                                    const LayerSpec::NeuronType neuron_type,
                                                    ActivationFunction* activation_func) 
    {

        std::vector<LayerSpec> layer_specs;

        if(num_hidden_layers == 0)
            layer_specs.push_back(LayerSpec(neuron_type, num_outputs, 
                                            num_inputs, activation_func));
        else 
        {

            layer_specs.push_back(LayerSpec(neuron_type, neurons_per_layer, 
                                            num_inputs, activation_func));

            for(unsigned i = 1; i < num_hidden_layers; i++)
                layer_specs.push_back(LayerSpec(neuron_type, neurons_per_layer, 
                                                neurons_per_layer, activation_func));

            layer_specs.push_back(LayerSpec(neuron_type, num_outputs, 
                                            neurons_per_layer, activation_func));

        }

        return layer_specs;

    }

    ~LayerSpec() = default;
    LayerSpec(LayerSpec&& layer_spec) = default;
    LayerSpec& operator=(LayerSpec&& layer_spec) = default;

    NeuronType get_neuron_type() const 
    {
        return _neuron_type;
    }

    unsigned get_num_neurons() const 
    {
        return _num_neurons;
    } 

    unsigned get_inputs_per_neuron() const
    {
        return _inputs_per_neuron;
    }

    unsigned get_params_per_neuron() const
    {
        return _params_per_neuron;
    }

    unsigned get_num_weights() const
    {
        return _num_neurons * _params_per_neuron;
    }

    const ActivationFunction* get_activation_func() const
    {
        return _activation_func;
    }

    void print_spec() const 
    {

        std::cout << "Layer Spec:" << std::endl;
        std::cout << "Neuron type: " << _neuron_type << std::endl;
        std::cout << "Num neurons: " << _num_neurons << std::endl;
        std::cout << "Inputs per neuron: " << _inputs_per_neuron << std::endl;
        std::cout << "Params per neuron: " << _params_per_neuron << std::endl;

    }

private: 

    NeuronType _neuron_type;
    unsigned _num_neurons;
    unsigned _inputs_per_neuron;
    unsigned _params_per_neuron;

    ActivationFunction* _activation_func;

};

} // namespace NeuroEvo

#endif
