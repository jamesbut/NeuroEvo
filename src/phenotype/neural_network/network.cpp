#include <phenotype/neural_network/network.h>
#include <fstream>

namespace NeuroEvo {

Network::Network(const bool trace) :
    Phenotype<double>(trace) {} 

Network::Network(const Network& network) :
    Phenotype<double>(network._trace),
    _layers(network._layers.size())
{
    for(std::size_t i = 0; i < _layers.size(); i++)
        _layers[i] = network._layers[i]->clone();
}

void Network::create_net(const std::vector<LayerSpec>& layer_specs) 
{
    for(const auto& layer_spec : layer_specs)
        _layers.push_back(
            std::unique_ptr<Layer>(new Layer(layer_spec, _trace))
        );

    for(const auto& layer : _layers)
        layer->create_layer();

    //Calculate and set number of weights
    unsigned num_weights = 0;
    for(const auto& layer : _layers)
        num_weights += layer->get_number_of_weights();
    _num_params = num_weights;
    
}

void Network::propogate_weights(const std::vector<double>& weights) 
{

    auto start = weights.begin();
    auto end = weights.begin();

    for(auto& layer : _layers)
    {

        end += layer->get_number_of_weights();

        std::vector<double> tempW(start, end);
        layer->set_weights(tempW);

        start += layer->get_number_of_weights();

    }

}

std::vector<double> Network::activate(const std::vector<double>& inputs) 
{
    return propogate(inputs);
}

void Network::reset()
{
    for(auto& layer : _layers)
        layer->reset();
}

void Network::set_trace(const bool trace)
{
    _trace = trace;
    for(const auto& layer : _layers)
        layer->set_trace(trace);
}

void Network::print_params()
{
    for(auto& layer : _layers)
        layer->print_params();
}

std::vector<double> Network::propogate(const std::vector<double>& inputs) 
{
    std::vector<double> ins = inputs;

    for(std::size_t i = 0; i < _layers.size(); i++) 
    {
        if(_trace) std::cout << "\nLayer: " << i << std::endl;
        ins = _layers[i]->evaluate(ins);
    }

    return ins;
}

} // namespace NeuroEvo
