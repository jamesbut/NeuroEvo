#include <phenotype/fixed_network/neuron.h>
#include <iostream>
#include <cmath>

namespace NeuroEvo {
namespace Phenotypes {

Neuron::Neuron(const LayerSpec& LAYER_SPEC, const bool TRACE) :
    _LAYER_SPEC(LAYER_SPEC),
    _previous_output(0.0),
    _TRACE(TRACE) {}

//Sets the weights of the Neuron
void Neuron::set_weights(std::vector<double>& weights) {

    _weights = weights;

    //if(_TRACE) print_weights();

}

double Neuron::evaluate(std::vector<double>& inputs) {

    double activation_val = 0.0;

    for(std::size_t i = 0; i < inputs.size(); i++) {

        //Print out execution
        if(_TRACE) std::cout << "\n" << inputs.at(i) << " x " << _weights.at(i);

        activation_val += inputs.at(i) * _weights.at(i);

    }

    //Recurrent input
    if(_LAYER_SPEC._type == 1) {

        activation_val += _previous_output * _weights.at(inputs.size());
        activation_val += 1 * _weights.at(inputs.size()+1);

        if(_TRACE) std::cout << "\n" << _previous_output << " x " << _weights.at(inputs.size()) << "\n";
        if(_TRACE) std::cout << "\n" << "1 x " << _weights.at(inputs.size()+1) << "\n\n";

    } else {

        activation_val += 1 * _weights.at(inputs.size());

        if(_TRACE) std::cout << "\n" << "1 x " << _weights.at(inputs.size()) << "\n\n";

    }


    double output = _LAYER_SPEC._activation_func->activate(activation_val);

    _previous_output = output;

    return output;

}

void Neuron::print_weights() {

    for(int i = 0; i < _weights.size(); i++)
        std::cout << _weights[i] << ' ';
    std::cout << "\n";

}

void Neuron::reset() {

    _previous_output = 0.0;

}

} // namespace Phenotypes
} // namespace NeuroEvo
