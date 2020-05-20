#ifndef _NETWORK_H_
#define _NETWORK_H_

/*
    Defines a network as a vector of layers
*/

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <phenotype/neural_network/layer.h>

namespace NeuroEvo {

template <typename G>
class Network : public Phenotype {

public:

    Network(const NetworkBuilder<G>& net_builder) :
        _net_builder(net_builder)
    {
        create_net();
    }

    Network(const std::vector<double>& traits, NetworkBuilder<G>& net_builder) :
        _net_builder(net_builder)
    {
        create_net();
        propogate_weights(traits);
    }

    void propogate_weights(const std::vector<double>& weights) 
    {

        auto start = weights.begin();
        auto end = weights.begin();

        for(auto& layer : _layers)
        {

            end += layer.get_number_of_params();

            std::vector<double> tempW(start, end);
            layer.set_weights(tempW);

            start += layer.get_number_of_params();

        }

    }

    std::vector<double> activate(const std::vector<double>& inputs) override 
    {

        std::vector<double> ins = inputs;

        for(unsigned i = 0; i < _net_builder.get_layer_specs().size(); i++) 
        {
            if(_net_builder.get_trace()) std::cout << "\nLayer: " << i << std::endl;
            ins = _layers[i].evaluate(ins);
        }

        return ins;

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
            layer.print_params();
    }

private:

    void create_net() 
    {
        for(unsigned i = 0; i < _net_builder.get_layer_specs().size(); i++)
            _layers.push_back(Layer(_net_builder.get_layer_specs()[i], 
                                    _net_builder.get_trace()));
    }

    const NetworkBuilder<G> _net_builder;

    std::vector<Layer> _layers;

};

} // namespace NeuroEvo

#endif
