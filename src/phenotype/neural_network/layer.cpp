#include <phenotype/fixed_network/layer.h>
#include <phenotype/fixed_network/gru_neuron.h>
#include <iostream>

namespace NeuroEvo {

Layer::Layer(const LayerSpec& layer_spec, const bool trace) :
    _layer_spec(layer_spec),
    _trace(trace) 
{

    if(_layer_spec._type == LayerSpec::NeuronType::GRU)
        for(unsigned int i = 0; i < _layer_spec._num_neurons; i++)
            _neurons.push_back(std::unique_ptr<Neuron>(new GRUNeuron(_layer_spec, _trace)));
    else
        for(unsigned int i = 0; i < _layer_spec._num_neurons; i++)
            _neurons.push_back(std::unique_ptr<Neuron>(new Neuron(_layer_spec, _trace)));

}

Layer::Layer(const Layer& layer) :
    _layer_spec(layer._layer_spec),
    _trace(layer._trace),
    _neurons(layer._layer_spec._num_neurons) 
{

    for(std::size_t i = 0; i < _neurons.size(); i++)
        _neurons[i] = layer._neurons[i]->clone();

}

void Layer::set_weights(std::vector<double>& weights) 
{

    std::vector<double>::iterator start = weights.begin();
    std::vector<double>::iterator end = weights.begin();

    for(const auto& neuron : _neurons)
    {

        end += _layer_spec._params_per_neuron;

        std::vector<double> tempW(start, end);
        neuron->set_weights(tempW);

        start += _layer_spec._params_per_neuron;

    }

}

unsigned Layer::get_number_of_params() const
{
    return _layer_spec._num_neurons * _layer_spec._params_per_neuron;
}

std::vector<double> Layer::evaluate(std::vector<double>& inputs) 
{

    std::vector<double> outputs;
    outputs.reserve(_neurons.size());

    for(const auto& neuron : _neurons)
    {
        if(_trace) std::cout << "Neuron: " << i << std::endl;
        outputs.push_back(neuron->evaluate(inputs));
    }

    //Print outputs
    if(_trace) {
        std::cout << "Layer outputs:" << std::endl;
        print_outputs(outputs);
    }

    return outputs;

}

void Layer::print_outputs(std::vector<double>& outputs) 
{

    std::cout << "\n";

    for(const auto& output : outputs)
        std::cout << output << " ";

    std::cout << "\n\n";

}

void Layer::reset() 
{
    for(auto& neuron : _neurons)
        neuron->reset();
}

void Layer::print_params() const
{
    for(const auto& neuron : _neurons)
        neuron->print_weights();
}

 } // namespace NeuroEvo
