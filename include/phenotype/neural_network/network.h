#ifndef _NETWORK_H_
#define _NETWORK_H_

/*
    Defines a network as a vector of layers
*/

#include <phenotype/phenotype.h>
#include <phenotype/neural_network/layer.h>
#include <fstream>

namespace NeuroEvo {

class Network : public Phenotype<double> 
{

public:

    Network(const bool trace = false) :
        Phenotype<double>(trace) {} 

    Network(const Network& network) :
        Phenotype<double>(network._trace),
        _layers(network._layers.size())
    {
        for(std::size_t i = 0; i < _layers.size(); i++)
            _layers[i] = network._layers[i]->clone();
    }

    virtual void create_net(const std::vector<LayerSpec>& layer_specs) 
    {
        for(const auto& layer_spec : layer_specs)
            _layers.push_back(
                std::unique_ptr<Layer>(new Layer(layer_spec, _trace))
            );

        for(const auto& layer : _layers)
            layer->create_layer();
    }

    void propogate_weights(const std::vector<double>& weights) 
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

    virtual void propogate_learning_rates(const std::vector<double>& learning_rates) {}

    virtual std::vector<double> activate(const std::vector<double>& inputs) override 
    {
        return propogate(inputs);
    }

    void reset() override
    {
        for(auto& layer : _layers)
            layer->reset();
    }

protected:

    virtual Network* clone_impl() const override 
    { 
        return new Network(*this); 
    };

    void print_params() override
    {
        for(auto& layer : _layers)
            layer->print_params();
    }

    std::vector<double> propogate(const std::vector<double>& inputs) 
    {
        std::vector<double> ins = inputs;

        for(std::size_t i = 0; i < _layers.size(); i++) 
        {
            if(_trace) std::cout << "\nLayer: " << i << std::endl;
            ins = _layers[i]->evaluate(ins);
        }

        return ins;
    }

    std::vector<std::unique_ptr<Layer>> _layers;

};

} // namespace NeuroEvo

#endif
