#include <phenotype/neural_network/neuron.h>
#include <iostream>
#include <cmath>
#include <fstream>

namespace NeuroEvo {

Neuron::Neuron(const unsigned num_inputs, const NeuronType& neuron_type,
               const std::shared_ptr<ActivationFunction> activation_function,
               const bool bias,
               const bool trace) :
    _num_inputs(num_inputs),
    _neuron_type(neuron_type),
    _activation_function(activation_function),
    _bias(bias),
    _trace(trace),
    _previous_output(0.0) {}

//Sets the weights of the Neuron
void Neuron::set_weights(const std::vector<double>& weights)
{
    check_num_weights(weights);
    _weights = weights;
}

void Neuron::set_trace(const bool trace)
{
    _trace = trace;
}

const std::vector<double>& Neuron::get_weights() const
{
    return _weights;
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

    if(_trace) std::cout << std::endl;

    //Recurrent input
    if(_neuron_type == NeuronType::Recurrent)
    {

        activation_val += _previous_output * _weights[inputs.size()];
        if(_bias)
            activation_val += _weights[inputs.size()+1];

        if(_trace)
        {
            std::cout << _previous_output << " x " << _weights[inputs.size()] << std::endl;
            if(_bias)
                std::cout << "1 x " << _weights[inputs.size()+1] << "\n\n";
        }

    } else
    {
        if(_bias)
        {
            activation_val += 1 * _weights[inputs.size()];
            if(_trace) std::cout << "1 x " << _weights[inputs.size()] << "\n\n";
        }
    }

    if(_trace) std::cout << std::endl;

    double output = activation_val;
    //Apply activation function if there is one there
    if(_activation_function)
        output = _activation_function->activate(activation_val);

    _previous_output = output;

    return output;

}

void Neuron::print(std::ostream& os) const
{
    for(const auto& weight : _weights)
        os << weight << " ";
    os << std::endl;
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

void Neuron::check_num_weights(const std::vector<double>& weights) const
{
    unsigned num_required_weights = _num_inputs;

    if(_bias)
        num_required_weights += 1;

    if(_neuron_type == NeuronType::Recurrent)
        num_required_weights += 1;

    if(weights.size() != num_required_weights)
    {
        std::cerr << "Giving wrong number of weights to neuron" << std::endl;
        exit(0);
    }

}

} // namespace NeuroEvo
