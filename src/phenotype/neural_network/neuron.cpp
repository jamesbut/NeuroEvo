#include <phenotype/fixed_network/neuron.h>
#include <iostream>
#include <cmath>

namespace NeuroEvo {

Neuron::Neuron(const LayerSpec& layer_spec, const bool trace) :
    _layer_spec(layer_spec),
    _previous_output(0.0),
    _trace(trace) {}

//Sets the weights of the Neuron
void Neuron::set_weights(std::vector<double>& weights) 
{
    _weights = weights;
}

double Neuron::evaluate(std::vector<double>& inputs) 
{

    double activation_val = 0.0;

    for(std::size_t i = 0; i < inputs.size(); i++) 
    {
        if(_trace) std::cout << "\n" << inputs[i] << " x " << _weights[i];
        activation_val += inputs[i] * _weights[i];
    }

    //Recurrent input
    if(_layer_spec.get_neuron_type() == LayerSpec::NeuronType::Recurrent) {

        activation_val += _previous_output * _weights[inputs.size()];
        activation_val += 1 * _weights[inputs.size()+1];

        if(_trace) 
            std::cout << "\n" << _previous_output << " x " << _weights[inputs.size()] << "\n";
        if(_trace) 
            std::cout << "\n" << "1 x " << _weights.at[inputs.size()+1] << "\n\n";

    } else {

        activation_val += 1 * _weights[inputs.size()];

        if(_trace) std::cout << "\n" << "1 x " << _weights.at(inputs.size()) << "\n\n";

    }


    const double output = _layer_spec.get_activation_func()->activate(activation_val);

    _previous_output = output;

    return output;

}

void Neuron::print_weights() 
{

    for(int i = 0; i < _weights.size(); i++)
        std::cout << _weights[i] << ' ';
    std::cout << "\n";

}

void Neuron::reset() 
{
    _previous_output = 0.0;
}

} // namespace NeuroEvo
