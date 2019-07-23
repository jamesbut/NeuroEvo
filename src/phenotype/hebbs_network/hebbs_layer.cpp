#include <phenotype/hebbs_network/hebbs_layer.h>
#include <iostream>

HebbsLayer::HebbsLayer(const LayerSpec& layer_spec, const bool TRACE) :
    _LAYER_SPEC(layer_spec),
    _TRACE(TRACE) {

    for(unsigned int i = 0; i < _LAYER_SPEC._num_neurons; i++)
        neurons.push_back(HebbsNeuron(_LAYER_SPEC, _TRACE));

}

void HebbsLayer::set_learning_rates(std::vector<double>& learning_rates) {

    std::vector<double>::iterator start = learning_rates.begin();
    std::vector<double>::iterator end = learning_rates.begin();

    for(unsigned int i = 0; i < _LAYER_SPEC._num_neurons; i++) {

        end += _LAYER_SPEC._params_per_neuron;

        std::vector<double> tempW(start, end);
        neurons.at(i).set_learning_rates(tempW);

        start += _LAYER_SPEC._params_per_neuron;

     }

}

void HebbsLayer::set_weights(std::vector<double>& weights) {

    std::vector<double>::iterator start = weights.begin();
    std::vector<double>::iterator end = weights.begin();

    for(unsigned int i = 0; i < _LAYER_SPEC._num_neurons; i++) {

        end += _LAYER_SPEC._params_per_neuron;

        std::vector<double> tempW(start, end);
        neurons.at(i).set_weights(tempW);

        start += _LAYER_SPEC._params_per_neuron;

     }

}

int HebbsLayer::get_number_of_connections() {

    return _LAYER_SPEC._num_neurons * _LAYER_SPEC._params_per_neuron;

}

std::vector<double> HebbsLayer::evaluate(std::vector<double>& inputs) {

    std::vector<double> outputs;

    for(std::size_t i = 0; i < neurons.size(); i++)
        outputs.push_back(neurons.at(i).evaluate(inputs));

    //Print outputs
    if(_TRACE) print_outputs(outputs);

    return outputs;

}

void HebbsLayer::print_outputs(std::vector<double>& outputs) {

    std::cout << "\n";

    for(std::size_t i = 0; i < outputs.size(); i++)
        std::cout << outputs.at(i) << " ";

    std::cout << "\n\n";

}

void HebbsLayer::print_weights() {

    for(std::size_t i = 0; i < neurons.size(); i++)
        neurons.at(i).print_weights();

}

void HebbsLayer::print_weights_to_file(std::ofstream& file) {

    for(std::size_t i = 0; i < neurons.size(); i++)
        neurons.at(i).print_weights_to_file(file);

}

void HebbsLayer::print_outputs_to_file(std::ofstream& file) {

    for(std::size_t i = 0; i < neurons.size(); i++)
        neurons.at(i).print_output_to_file(file);

}

void HebbsLayer::reset() {

    for(auto& neuron : neurons)
        neuron.reset();

}
