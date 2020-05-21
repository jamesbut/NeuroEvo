#include <phenotype/neural_network/layer.h>
#include <phenotype/neural_network/gru_neuron.h>
#include <iostream>

namespace NeuroEvo {

Layer::Layer(const LayerSpec& layer_spec, const bool trace) :
    _layer_spec(layer_spec),
    _trace(trace) 
{

    if(_layer_spec.get_neuron_type() == LayerSpec::NeuronType::GRU)
        for(unsigned i = 0; i < _layer_spec.get_num_neurons(); i++)
            _neurons.push_back(std::unique_ptr<Neuron>(new GRUNeuron(_layer_spec, _trace)));
    else
        for(unsigned i = 0; i < _layer_spec.get_num_neurons(); i++)
            _neurons.push_back(std::unique_ptr<Neuron>(new Neuron(_layer_spec, _trace)));

}

Layer::Layer(const Layer& layer) :
    _layer_spec(layer._layer_spec),
    _trace(layer._trace),
    _neurons(layer._layer_spec.get_num_neurons()) 
{

    for(std::size_t i = 0; i < _neurons.size(); i++)
        _neurons[i] = layer._neurons[i]->clone();

}

void Layer::set_weights(const std::vector<double>& weights) 
{

    auto start = weights.begin();
    auto end = weights.begin();

    for(const auto& neuron : _neurons)
    {

        end += _layer_spec.get_params_per_neuron();

        std::vector<double> tempW(start, end);
        neuron->set_weights(tempW);

        start += _layer_spec.get_params_per_neuron();

    }

}

unsigned Layer::get_number_of_weights() const
{
    return _layer_spec.get_num_neurons() * _layer_spec.get_params_per_neuron();
}

std::vector<double> Layer::evaluate(const std::vector<double>& inputs) 
{

    std::vector<double> outputs;
    outputs.reserve(_neurons.size());

    for(std::size_t i = 0; i < _neurons.size(); i++)
    {
        if(_trace) std::cout << "Neuron: " << i << std::endl;
        outputs.push_back(_neurons[i]->evaluate(inputs));
    }

    //Print outputs
    if(_trace) {
        std::cout << "Layer outputs:" << std::endl;
        print_outputs(outputs);
    }

    return outputs;

}

void Layer::print_outputs(const std::vector<double>& outputs) 
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

void Layer::print_weights() const
{
    for(const auto& neuron : _neurons)
        neuron->print_weights();
}

void Layer::print_params() const
{
    print_weights();
}

void Layer::print_weights_to_file(std::ofstream& file) const
{
    for(const auto& neuron : _neurons)
        neuron->print_weights_to_file(file);
}

void Layer::print_outputs_to_file(std::ofstream& file) const 
{
    for(const auto& neuron : _neurons)
        neuron->print_output_to_file(file);
}

} // namespace NeuroEvo
