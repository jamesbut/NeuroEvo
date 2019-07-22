#include <phenotype/fixed_network/network.h>

FixedNetwork::FixedNetwork(const FixedNetworkSpec NET_SPEC) :
    _NET_SPEC(NET_SPEC),
    _TRACE(NET_SPEC.TRACE) {

    create_net();

}

FixedNetwork::FixedNetwork(const std::vector<double>& traits, FixedNetworkSpec& NET_SPEC) :
    _NET_SPEC(NET_SPEC),
    _TRACE(NET_SPEC.TRACE) {

    create_net();

    propogate_weights(traits);

}

void FixedNetwork::create_net() {

    for(unsigned i = 0; i < _NET_SPEC.LAYER_SPECS.size(); i++)
        layers.push_back(Layer(_NET_SPEC.LAYER_SPECS.at(i), _TRACE));

}

void FixedNetwork::propogate_weights(const std::vector<double>& weights) {

    auto start = weights.begin();
    auto end = weights.begin();

    for(std::size_t i = 0; i < layers.size(); i++) {

        end += layers[i].get_number_of_params();

        std::vector<double> tempW(start, end);
        layers[i].set_weights(tempW);

        start += layers[i].get_number_of_params();

    }

}

std::vector<double> FixedNetwork::activate(std::vector<double>& inputs) {

    std::vector<double> ins = inputs;

    for(unsigned int i = 0; i < _NET_SPEC.LAYER_SPECS.size(); i++)
        ins = layers.at(i).evaluate(ins);

    return ins;

}

void FixedNetwork::reset() {

    for(auto& layer : layers)
        layer.reset();

}
