#ifndef _NETWORK_H_
#define _NETWORK_H_

/*
    Defines a network as a vector of layers
*/

#include <phenotype/phenotype.h>
#include <phenotype/neural_network/layer.h>
#include <fstream>

namespace NeuroEvo {

class Network : public Phenotype {

public:

    Network(const std::vector<LayerSpec>& layer_specs,
            const bool trace = false) :
        _trace(trace)
    {
        //TODO: Can't call this here!!
        std::cerr << "See Network constructor!" << std::endl;
        exit(0);
        create_net(layer_specs, trace);
    }

    Network(const std::vector<double>& traits, 
            const std::vector<LayerSpec>& layer_specs,
            const bool trace = false) :
        _trace(trace)
    {
        //TODO: Can't call this here!!
        std::cerr << "See Network constructor!" << std::endl;
        exit(0);
        create_net(layer_specs, trace);
        propogate_weights(traits);
    }

    Network(const Network& network) :
        _trace(network._trace)
    {
        for(std::size_t i = 0; network._layers.size(); i++)
            _layers[i] = network._layers[i]->clone();
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

    virtual std::vector<double> activate(const std::vector<double>& inputs) override 
    {
        return propogate(inputs);
    }

    void reset() override
    {
        for(auto& layer : _layers)
            layer.reset();
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

    virtual void create_net(const std::vector<LayerSpec>& layer_specs, const bool trace) 
    {
        for(const auto& layer_spec : layer_specs)
            _layers.push_back(
                std::unique_ptr<Layer>(new Layer(layer_spec, trace))
            );
    }

    const bool _trace;

    std::vector<std::unique_ptr<Layer>> _layers;

};

} // namespace NeuroEvo

#endif
