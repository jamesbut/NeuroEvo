#include <phenotype/neural_network/hebbs_neuron.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>

namespace NeuroEvo {

//Constructor
HebbsNeuron::HebbsNeuron(const unsigned num_inputs, 
                         const LayerSpec::NeuronType& neuron_type, 
                         const std::unique_ptr<ActivationFunction>& activation_function,
                         const bool trace) :
    Neuron(neuron_type, neuron_type, activation_function, trace) {}

void HebbsNeuron::set_weights(const std::vector<double>& weights) 
{

    _weights = weights;

    //Normalise weights
    normalise_weights();

}

void HebbsNeuron::set_learning_rates(const std::vector<double>& learning_rates) 
{
    _learning_rates = learning_rates;
}

double HebbsNeuron::evaluate(const std::vector<double>& inputs) 
{
    //Normalise Hebbs - normalise before using weights in evaluate
    normalise_weights();

    const double output = propogate(inputs);

    //Allow for synaptic weight change - this is what makes it a plastic network
    synaptic_weight_change(inputs, output);

    return output;
}

void HebbsNeuron::synaptic_weight_change(const std::vector<double>& inputs,
                                         const double output) 
{   
    //Simple Hebbs
    //Here I am assuming that each synapse has a different learning rate
    //and these are the learning parameters that are evolved
    for(std::size_t i = 0; i < inputs.size(); i++) 
    {
        const double delta_w = _learning_rates[i] * inputs[i] * output;
        _weights[i] += delta_w;
    }

    if(_neuron_type == LayerSpec::NeuronType::Recurrent) 
    {

        const double delta_w_recurrent = _learning_rates[inputs.size()] * 
                                         _previous_output * output;
        _weights[inputs.size()] += delta_w_recurrent;

        //Make sure to change bias as well
        const double delta_w_bias = _learning_rates[inputs.size()+1] * output;
        _weights[inputs.size()+1] += delta_w_bias;

    } else 
    {
        const double delta_w_bias = _learning_rates[inputs.size()] * output;
        _weights[inputs.size()] += delta_w_bias;
    }

}

void HebbsNeuron::normalise_weights() 
{
    double sum_of_square_weights = 0.0;

    for(const auto& weight : _weights)
        sum_of_square_weights += weight * weight;

    const double normalising_constant = sqrt(sum_of_square_weights);

    for(auto& weight : _weights)
        weight /= normalising_constant;

}

void HebbsNeuron::reset() 
{

    _previous_output = 0.0;

    //TODO: Set weights back to intial values too

}

} // namespace NeuroEvo
