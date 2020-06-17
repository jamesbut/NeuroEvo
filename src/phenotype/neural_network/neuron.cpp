#include <phenotype/neural_network/neuron.h>
#include <iostream>
#include <cmath>
#include <fstream>

namespace NeuroEvo {

Neuron::Neuron(const unsigned num_inputs, const LayerSpec::NeuronType& neuron_type, 
               const std::shared_ptr<ActivationFunction> activation_function,
               const bool trace) :
    _num_inputs(num_inputs),
    _neuron_type(neuron_type),
    _activation_function(activation_function),
    _trace(trace),
    _previous_output(0.0) {}

//Sets the weights of the Neuron
void Neuron::set_weights(const std::vector<double>& weights) 
{
    _weights = weights;
}

double Neuron::evaluate(const std::vector<double>& inputs) 
{
    return propogate(inputs);
}

double Neuron::propogate(const std::vector<double>& inputs)
{
    double activation_val = 0.0;

    for(std::size_t i = 0; i < inputs.size(); i++) 
    {
        if(_trace) std::cout << "\n" << inputs[i] << " x " << _weights[i];
        activation_val += inputs[i] * _weights[i];
    }

    //Recurrent input
    if(_neuron_type == LayerSpec::NeuronType::Recurrent) {

        activation_val += _previous_output * _weights[inputs.size()];
        activation_val += _weights[inputs.size()+1];

        if(_trace) 
        {
            std::cout << "\n" << _previous_output << " x " << _weights[inputs.size()];
            std::cout << "\n" << "1 x " << _weights[inputs.size()+1] << "\n\n";
        }

    } else {

        activation_val += 1 * _weights[inputs.size()];

        if(_trace) std::cout << "\n" << "1 x " << _weights[inputs.size()] << "\n\n";

    }

    const double output = _activation_function->activate(activation_val);

    _previous_output = output;

    return output;

}

void Neuron::print_weights() const
{
    for(const auto& weight : _weights)
        std::cout << weight << " ";
    std::cout << std::endl;
}

void Neuron::print_weights_to_file(std::ofstream& file) const
{
    for(const auto& weight : _weights)
        file << weight << ",";
}

void Neuron::print_output_to_file(std::ofstream& file) const
{
    file << _previous_output << ",";
}

void Neuron::reset() 
{
    _previous_output = 0.0;
}

} // namespace NeuroEvo
