#include <phenotype/neural_network/layer.h>
#include <phenotype/neural_network/gru_neuron.h>
#include <iostream>

namespace NeuroEvo {

Layer::Layer(const LayerSpec& layer_spec, const bool trace) :
    _trace(trace),
    _inputs_per_neuron(layer_spec.get_inputs_per_neuron()),
    _params_per_neuron(layer_spec.get_params_per_neuron()),
    _num_neurons(layer_spec.get_num_neurons()),
    _neuron_type(layer_spec.get_neuron_type()),
    _activation_function(layer_spec.get_activation_func_spec() ?
                         layer_spec.get_activation_func_spec()->create_activation_function() :
                         nullptr),
    _bias(layer_spec.get_bias()) {}

Layer::Layer(const Layer& layer) :
    _trace(layer._trace),
    _inputs_per_neuron(layer._inputs_per_neuron),
    _params_per_neuron(layer._params_per_neuron),
    _num_neurons(layer._num_neurons),
    _neuron_type(layer._neuron_type),
    _activation_function(layer._activation_function ? 
                         layer._activation_function->clone() : nullptr),
    _bias(layer._bias),
    _neurons(layer._num_neurons) 
{
    for(std::size_t i = 0; i < _neurons.size(); i++)
        _neurons[i] = layer._neurons[i]->clone();
}

void Layer::create_layer()
{
    if(_neuron_type == NeuronType::GRU)
        for(unsigned i = 0; i < _num_neurons; i++)
            _neurons.push_back(std::unique_ptr<Neuron>(new GRUNeuron(_inputs_per_neuron, 
                                                                     _activation_function,
                                                                     _trace)));
    else
        for(unsigned i = 0; i < _num_neurons; i++)
            _neurons.push_back(std::unique_ptr<Neuron>(new Neuron(_inputs_per_neuron, 
                                                                  _neuron_type,
                                                                  _activation_function,
                                                                  _bias,
                                                                  _trace)));
}

void Layer::set_weights(const std::vector<double>& weights) 
{

    auto start = weights.begin();
    auto end = weights.begin();

    for(const auto& neuron : _neurons)
    {

        end += _params_per_neuron;

        std::vector<double> tempW(start, end);
        neuron->set_weights(tempW);

        start += _params_per_neuron;

    }

}

void Layer::set_trace(const bool trace)
{
   _trace = trace; 
   for(const auto& neuron : _neurons)
       neuron->set_trace(trace);
}

unsigned Layer::get_number_of_weights() const
{
    return _num_neurons * _params_per_neuron;
}

std::vector<double> Layer::get_weights() const
{
    std::vector<double> weights; 
    for(const auto& neuron : _neurons)
    {
        auto neuron_weights = neuron->get_weights();
        weights.insert(weights.end(), neuron_weights.begin(), neuron_weights.end());
    }
    return weights;
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
