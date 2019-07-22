#ifndef _LAYER_SPEC_H_
#define _LAYER_SPEC_H_

#include <math.h>
#include <memory>
#include <iostream>

/*
    Defines an activation function description
    and implements that activation function.
*/

struct ActivationFuncSpec {

    virtual double activate(const double x) = 0;

    auto clone() const { return std::unique_ptr<ActivationFuncSpec>(clone_impl()); };
    virtual void print_spec() const = 0;

protected:

    virtual ActivationFuncSpec* clone_impl() const = 0;

};

struct SigmoidSpec : ActivationFuncSpec {

    SigmoidSpec(const double K) :
        K(K) {}

    const double K;

    double activate(const double x) override {

        return 1 / (1 + exp(-x / K));

    }

    void print_spec() const {
        std::cout << "K: " << K << std::endl;
    }

protected:

    SigmoidSpec* clone_impl() const override { return new SigmoidSpec(*this); };

};

/*
    Defines a network layer.
    Only normal, recurrent and GRU units can be used currently.
*/

struct LayerSpec {

    LayerSpec(const unsigned type, const unsigned num_neurons,
              unsigned inputs_per_neuron, ActivationFuncSpec* activation_spec) :
        _type(type),
        _num_neurons(num_neurons),
        _inputs_per_neuron(inputs_per_neuron),
        _activation_spec(activation_spec) {

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
        _activation_spec(layer_spec._activation_spec.get()->clone()) {}

    LayerSpec& operator=(const LayerSpec& layer_spec) {

        _type = layer_spec._type;
        _num_neurons = layer_spec._num_neurons;
        _inputs_per_neuron = layer_spec._inputs_per_neuron;
        _params_per_neuron = layer_spec._params_per_neuron;
        _activation_spec = layer_spec._activation_spec.get()->clone();

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
        _activation_spec->print_spec();

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

    std::unique_ptr<ActivationFuncSpec> _activation_spec;

};

#endif
