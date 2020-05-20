#ifndef _NETWORK_H_
#define _NETWORK_H_

/*
    Defines a fixed network as a vector of layers.
*/

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <phenotype/fixed_network/layer.h>

namespace NeuroEvo {

template <typename G>
class FixedNetwork : public Phenotype {

public:

    FixedNetwork(const FixedNetworkSpec<G>& net_spec) :
        _net_spec(net_spec),
        _trace(net_spec.trace) 
    {
        create_net();
    }

    FixedNetwork(const std::vector<double>& traits, FixedNetworkSpec<G>& net_spec) :
        _net_spec(net_spec),
        _trace(net_spec.trace) 
    {
        create_net();
        propogate_weights(traits);
    }

    void propogate_weights(const std::vector<double>& weights) 
    {

        auto start = weights.begin();
        auto end = weights.begin();

        for(std::size_t i = 0; i < _layers.size(); i++) 
        {

            end += _layers[i].get_number_of_params();

            std::vector<double> tempW(start, end);
            _layers[i].set_weights(tempW);

            start += _layers[i].get_number_of_params();

        }

    }

    std::vector<double> activate(const std::vector<double>& inputs) override 
    {

        std::vector<double> ins = inputs;

        for(unsigned int i = 0; i < _net_spec._layer_specs.size(); i++) 
        {
            if(_trace) std::cout << "\nLayer: " << i << std::endl;
            ins = _layers.at(i).evaluate(ins);
        }

        return ins;

    }

    void reset() override
    {
        for(auto& layer : _layers)
            layer.reset();
    }

protected:

    virtual FixedNetwork* clone_impl() const override { return new FixedNetwork(*this); };

    void print_params() override
    {
        for(auto& layer : layers)
            layer.print_params();
    }

private:

    void create_net() 
    {
        for(unsigned i = 0; i < _net_spec._layer_specs.size(); i++)
            _layers.push_back(Layer(_net_spec._layer_specs.at(i), _trace));
    }

    const FixedNetworkSpec<G> _net_spec;

    const bool _trace;

    std::vector<Layer> _layers;

};

} // namespace NeuroEvo

#endif
