#include <phenotype/neural_network/network.h>
#include <fstream>

namespace NeuroEvo {

Network::Network(const bool trace) :
    NetworkBase(trace) {}

Network::Network(const Network& network) :
    NetworkBase(network._trace),
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

    //Calculate and set num inputs and outputs
    _num_inputs = layer_specs[0].get_inputs_per_neuron();
    _num_outputs = layer_specs.back().get_num_neurons();

    //Set final layer activation function
    _final_layer_activ_func = _layers.back()->get_activation_function();

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

void Network::print(std::ostream& os) const
{
    for(auto& layer : _layers)
        layer->print(os);
}

std::vector<double> Network::get_weights() const
{
    std::vector<double> weights;
    for(const auto& layer : _layers)
    {
        auto layer_weights = layer->get_weights();
        weights.insert(weights.end(), layer_weights.begin(), layer_weights.end());
    }
    return weights;
}

const std::vector<std::unique_ptr<Layer>>& Network::get_layers() const
{
    return _layers;
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

JSON Network::to_json_impl() const
{
    JSON json;
    json.emplace("name", "Network");
    for(std::size_t i = 0; i < _layers.size(); i++)
        json.emplace("Layer" + std::to_string(i), _layers.at(i)->to_json().at());
    return json;
}

Network* Network::clone_impl() const
{
    return new Network(*this);
};

std::vector<double> Network::get_params() const
{
    return this->get_weights();
}

} // namespace NeuroEvo
