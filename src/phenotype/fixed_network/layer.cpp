#include <phenotype/fixed_network/layer.h>
#include <phenotype/fixed_network/gru_neuron.h>
#include <iostream>

namespace NeuroEvo {
namespace Phenotypes {

Layer::Layer(const LayerSpec& LAYER_SPEC, const bool TRACE) :
    _LAYER_SPEC(LAYER_SPEC),
    _TRACE(TRACE) {

    if(_LAYER_SPEC._type == 2)
        for(unsigned int i = 0; i < _LAYER_SPEC._num_neurons; i++)
            neurons.push_back(std::unique_ptr<Neuron>(new GRUNeuron(_LAYER_SPEC, _TRACE)));
    else
        for(unsigned int i = 0; i < _LAYER_SPEC._num_neurons; i++)
            neurons.push_back(std::unique_ptr<Neuron>(new Neuron(_LAYER_SPEC, _TRACE)));

}

Layer::Layer(const Layer& layer) :
    _LAYER_SPEC(layer._LAYER_SPEC),
    _TRACE(layer._TRACE),
    neurons(layer._LAYER_SPEC._num_neurons) {

    for(std::size_t i = 0; i < neurons.size(); i++)
        neurons.at(i) = layer.neurons.at(i)->clone();

}

void Layer::set_weights(std::vector<double>& weights) {

    std::vector<double>::iterator start = weights.begin();
    std::vector<double>::iterator end = weights.begin();

    for(unsigned int i = 0; i < _LAYER_SPEC._num_neurons; i++) {

        end += _LAYER_SPEC._params_per_neuron;

        std::vector<double> tempW(start, end);
        neurons.at(i)->set_weights(tempW);

        start += _LAYER_SPEC._params_per_neuron;

    }

}

unsigned Layer::get_number_of_params() {

    return _LAYER_SPEC._num_neurons * _LAYER_SPEC._params_per_neuron;

}

std::vector<double> Layer::evaluate(std::vector<double>& inputs) {

    std::vector<double> outputs;

    for(std::size_t i = 0; i < neurons.size(); i++) {
        if(_TRACE) std::cout << "Neuron: " << i << std::endl;
        outputs.push_back(neurons.at(i)->evaluate(inputs));
    }

    //Print outputs
    if(_TRACE) {
        std::cout << "Layer outputs:" << std::endl;
        print_outputs(outputs);
    }

    return outputs;

}

void Layer::print_outputs(std::vector<double>& outputs) {

    std::cout << "\n";

    for(std::size_t i = 0; i < outputs.size(); i++)
        std::cout << outputs.at(i) << " ";

    std::cout << "\n\n";

}
 void Layer::reset() {

    for(auto& neuron : neurons)
        neuron->reset();

 }

 } // namespace Phenotypes
 } // namespace NeuroEvo
