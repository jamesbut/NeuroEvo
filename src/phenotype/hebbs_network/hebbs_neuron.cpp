#include <phenotype/hebbs_network/hebbs_neuron.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>

namespace NeuroEvo {
namespace Phenotypes {

//Constructor
HebbsNeuron::HebbsNeuron(const LayerSpec& LAYER_SPEC, const bool TRACE) :
    _LAYER_SPEC(LAYER_SPEC),
    _previous_output(0.0),
    _TRACE(TRACE) {}

void HebbsNeuron::set_weights(std::vector<double>& weights) {

    _weights = weights;

    //Normalise weights
    normalise_weights();

}

void HebbsNeuron::set_learning_rates(std::vector<double>& learning_rates) {

    _learning_rates = learning_rates;

}

double HebbsNeuron::evaluate(std::vector<double>& inputs) {

    //Normalise Hebbs - normalise before using weights in evaluate
    normalise_weights();

    double activation_val = 0.0;

    for(std::size_t i = 0; i < inputs.size(); i++) {

        //Print out execution
        if(_TRACE) std::cout << "\n" << inputs.at(i) << " x " << _weights.at(i);

        activation_val += inputs.at(i) * _weights.at(i);

    }

    //Recurrent input
    if(_LAYER_SPEC._type == 1) {

        activation_val += _previous_output * _weights.at(inputs.size());
        activation_val += -1 * _weights.at(inputs.size()+1);

        if(_TRACE) std::cout << "\n" << _previous_output << " x " << _weights.at(inputs.size()) << "\n";
        if(_TRACE) std::cout << "\n" << "-1 x " << _weights.at(inputs.size()+1) << "\n";

    } else {

        //Bias input
        activation_val += -1 * _weights.at(inputs.size());

        if(_TRACE) std::cout << "\n" << "-1 x " << _weights.at(inputs.size()) << "\n";

    }

    double output = _LAYER_SPEC._activation_spec->activate(activation_val);

    //Allow for synaptic weight change - this is what makes it a plastic network
    synaptic_weight_change(inputs, output);

    _previous_output = output;

    return output;

}

void HebbsNeuron::synaptic_weight_change(const std::vector<double>& inputs,
                                    const double output) {

    //Simple Hebbs
    //Here I am assuming that each synapse has a different learning rate
    //and these are the learning parameters that are evolved
    for(std::size_t i = 0; i < inputs.size(); i++) {

        double delta_w = _learning_rates.at(i) * inputs.at(i) * output;
        _weights.at(i) += delta_w;

    }

    if(_LAYER_SPEC._type == 1) {

        double delta_w_recurrent = _learning_rates.at(inputs.size()) * _previous_output * output;
        _weights.at(inputs.size()) += delta_w_recurrent;

        //Make sure to change bias as well
        double delta_w_bias = _learning_rates.at(inputs.size()+1) * -1 * output;
        _weights.at(inputs.size()+1) += delta_w_bias;

    } else {

        double delta_w_bias = _learning_rates.at(inputs.size()) * -1 * output;
        _weights.at(inputs.size()) += delta_w_bias;

    }

}

void HebbsNeuron::normalise_weights() {

    double sum_of_square_weights = 0.0;

    for(std::size_t i = 0; i < _weights.size(); i++)
        sum_of_square_weights += _weights.at(i) * _weights.at(i);

    double normalising_constant = sqrt(sum_of_square_weights);

    for(std::size_t i = 0; i < _weights.size(); i++)
        _weights.at(i) /= normalising_constant;

}

void HebbsNeuron::print_weights() {

    for(int i = 0; i < _weights.size(); i++)
        std::cout << _weights[i] << ' ';
    std::cout << "\n";

}

void HebbsNeuron::print_weights_to_file(std::ofstream& file) {

    for(std::size_t i = 0; i < _weights.size(); i++)
        file << _weights.at(i) << ",";

}

void HebbsNeuron::print_output_to_file(std::ofstream& file) {

    file << _previous_output << ",";

}

void HebbsNeuron::reset() {

    _previous_output = 0.0;

    //TODO: Set weights back to intial values too

}

} // namespace Phenotypes
} // namespace NeuroEvo
