#ifndef _LAYER_SPEC_H_
#define _LAYER_SPEC_H_

#include <math.h>
#include <memory>
#include <iostream>
#include <util/maths/activation_functions/activation_function.h>

/*
    Defines an activation function description
    and implements that activation function.
*/

namespace NeuroEvo {
namespace Phenotypes {

/*
    Defines a network layer.
    Only normal, recurrent and GRU units can be used currently.
*/

struct LayerSpec {

    LayerSpec(const unsigned type, const unsigned num_neurons,
              unsigned inputs_per_neuron, Utils::ActivationFunction* activation_func) :
        _type(type),
        _num_neurons(num_neurons),
        _inputs_per_neuron(inputs_per_neuron),
        _activation_func(activation_func) {

    //Normal
    if(type == 0)
        _params_per_neuron = inputs_per_neuron + 1;

    //Recurrent
    if(_type == 1)
        _params_per_neuron = inputs_per_neuron + 2;

    //GRU
    if(_type == 2)
        _params_per_neuron = (inputs_per_neuron * 3) + 6;

}

    LayerSpec(const LayerSpec& layer_spec) :
        _type(layer_spec._type),
        _num_neurons(layer_spec._num_neurons),
        _inputs_per_neuron(layer_spec._inputs_per_neuron),
        _params_per_neuron(layer_spec._params_per_neuron),
        _activation_func(layer_spec._activation_func.get()->clone()) {}

    LayerSpec& operator=(const LayerSpec& layer_spec) {

        _type = layer_spec._type;
        _num_neurons = layer_spec._num_neurons;
        _inputs_per_neuron = layer_spec._inputs_per_neuron;
        _params_per_neuron = layer_spec._params_per_neuron;
        _activation_func = layer_spec._activation_func.get()->clone();

        return *this;

    }

    ~LayerSpec() = default;
    LayerSpec(LayerSpec&& layer_spec) = default;
    LayerSpec& operator=(LayerSpec&& layer_spec) = default;

    void print_spec() const {

        std::cout << "Layer Spec:" << std::endl;
        std::cout << "Type: " << _type << std::endl;
        std::cout << "Num neurons: " << _num_neurons << std::endl;
        std::cout << "Inputs per neuron: " << _inputs_per_neuron << std::endl;
        std::cout << "Params per neuron: " << _params_per_neuron << std::endl;

    }

    /*
        0: Standard
        1: Recurrent
        2: GRU
    */
    unsigned _type;
    unsigned _num_neurons;
    unsigned _inputs_per_neuron;
    unsigned _params_per_neuron;

    std::unique_ptr<Utils::ActivationFunction> _activation_func;

};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
