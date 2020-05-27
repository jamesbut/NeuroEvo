#ifndef _HEBBS_NETWORK_H_
#define _HEBBS_NETWORK_H_

/*
    The weights of a Hebbs network change online
    via Hebbs rule.
    The evolutionary process optimises the learning coefficients,
    not the weights themselves.
    This implementation also supports evolving the initial weights
    as oppose to just the Hebbs coefficients.
*/

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <phenotype/phenotype_specs/hebbs_spec.h>
#include <phenotype/neural_network/hebbs_layer.h>
#include <phenotype/neural_network/network.h>
#include <iostream>
#include <fstream>

namespace NeuroEvo {

class HebbsNetwork : public Network {

public:

    HebbsNetwork(const std::vector<LayerSpec>& layer_specs,
                 const HebbsSpec& hebbs_spec,
                 const bool trace = false) :
        _hebbs_spec(hebbs_spec),
        Network(layer_specs, trace)
    {
        create_net(layer_specs, trace);
    }
    
    HebbsNetwork(const std::vector<double>& traits, 
                 const std::vector<LayerSpec>& layer_specs,
                 const HebbsSpec& hebbs_spec,
                 const bool trace = false) :
        _hebbs_spec(hebbs_spec),
        Network(layer_specs, trace)
    {

        create_net(layer_specs, trace);

        //Work out number of total connections
        unsigned num_connections = 0;

        for(const auto& layer : _layers)
            num_connections += layer->get_number_of_weights();

        //If evolving weights as well as learning rates
        if(_hebbs_spec.get_evolve_init_weights()) 
        {

            //Split genes into 2 - learning rates and initial weights
            const std::vector<double> learning_rates(traits.begin(),
                                                     traits.begin() + num_connections);

            const std::vector<double> weights(traits.begin() + num_connections,
                                              traits.end());

            propogate_learning_rates(learning_rates);
            propogate_weights(weights);

        } else   //If just evolving learning rates
        {    

            std::vector<double> weights;
            weights.reserve(num_connections);

            //If the weights are randomly initialised or all set to some value
            if(!_hebbs_spec.get_default_weight_init()) 
            {

                UniformRealDistribution _uniform_distr(0., 1.);

                for(unsigned i = 0; i < num_connections; i++)
                    weights.push_back(_uniform_distr.next());

            } else 
            {
                for(unsigned i = 0; i < num_connections; i++)
                    weights.push_back(*_hebbs_spec.get_default_weight_init());
            }

            propogate_learning_rates(traits);
            propogate_weights(weights);

        }

    }

    void propogate_learning_rates(const std::vector<double>& learning_rates) 
    {
        auto start = learning_rates.begin();
        auto end = learning_rates.begin();

        //Dynamically cast to HebbsLayer in order to call Hebbs related methods
        std::vector<HebbsLayer*> hebbs_layers;
        hebbs_layers.reserve(_layers.size());
        for(auto& layer : _layers)
            hebbs_layers.push_back(static_cast<HebbsLayer*>(layer.get()));

        for(auto& layer : hebbs_layers)
        {
            end += layer->get_number_of_weights();

            const std::vector<double> tempW(start, end);
            layer->set_learning_rates(tempW);

            start += layer->get_number_of_weights();
        }
    }

    std::vector<double> activate(const std::vector<double>& inputs) override
    {
        if(_hebbs_spec.get_print_weights_to_file())
        {
            print_weights_to_file();
            print_outputs_to_file();
        }

        return propogate(inputs);
    }

protected:

    virtual HebbsNetwork* clone_impl() const override 
    { 
        return new HebbsNetwork(*this); 
    };

private:

    void create_net(const std::vector<LayerSpec>& layer_specs, const bool trace) override
    {
        for(const auto& layer_spec : layer_specs)
            _layers.push_back(
                std::unique_ptr<HebbsLayer>(new HebbsLayer(layer_spec, trace))
            );
    }

    void print_weights_to_file() const
    {

        std::ofstream weight_file;
        weight_file.open(_hebbs_spec.get_weights_file_name(), 
                         std::fstream::app);

        for(const auto& layer : _layers)
            layer->print_weights_to_file(weight_file);

        weight_file << std::endl;

        weight_file.close();

    }

    void print_outputs_to_file() const
    {

        std::ofstream output_file;
        output_file.open(_hebbs_spec.get_outputs_file_name(), 
                         std::fstream::app);

        for(const auto& layer : _layers)
            layer->print_outputs_to_file(output_file);

        output_file << std::endl;

        output_file.close();

    }

    const HebbsSpec& _hebbs_spec;

};

} // namespace NeuroEvo

#endif
