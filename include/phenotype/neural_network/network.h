#ifndef _NETWORK_H_
#define _NETWORK_H_

/*
    Defines a network as a vector of layers
*/

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <phenotype/neural_network/layer.h>
#include <fstream>

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

            end += layer.get_number_of_weights();

            std::vector<double> tempW(start, end);
            layer.set_weights(tempW);

            start += layer.get_number_of_weights();

        }

    }

    std::vector<double> activate(const std::vector<double>& inputs) override 
    {

        if(_net_builder.get_hebbs_spec().get_print_weights_to_file())
        {
            print_weights_to_file();
            print_outputs_to_file();
        }

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

    virtual void create_net() 
    {
        for(unsigned i = 0; i < _net_builder.get_layer_specs().size(); i++)
            _layers.push_back(Layer(_net_builder.get_layer_specs()[i], 
                                    _net_builder.get_trace()));
    }
    
    void print_weights_to_file() const
    {

        std::ofstream weight_file;
        weight_file.open(_net_builder.get_hebbs_spec().get_weights_file_name(), 
                         std::fstream::app);

        for(const auto& layer : _layers)
            layer.print_weights_to_file(weight_file);

        weight_file << std::endl;

        weight_file.close();

    }

    void print_outputs_to_file() const
    {

        std::ofstream output_file;
        output_file.open(_net_builder.get_hebbs_spec().get_outputs_file_name(), 
                         std::fstream::app);

        for(const auto& layer : _layers)
            layer.print_outputs_to_file(output_file);

        output_file << std::endl;

        output_file.close();

    }

    //TODO: This really should be a reference because it is getting larger!!
    const NetworkBuilder<G> _net_builder;

    std::vector<Layer> _layers;

};

} // namespace NeuroEvo

#endif
