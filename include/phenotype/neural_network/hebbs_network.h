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
#include <phenotype/neural_network/hebbs_layer.h>
#include <phenotype/neural_network/network.h>
#include <iostream>
#include <fstream>

namespace NeuroEvo {

template <typename G>
class HebbsNetwork : public Network<G> {

public:

    HebbsNetwork(const NetworkBuilder<G>& net_builder) :
        _net_spec(net_spec),
        _print_weights(net_spec.print_weights),
    {
        create_net();
    }
    
    HebbsNetwork(const std::vector<double>& traits, const NetworkBuilder<G>& net_builder) :
        _net_spec(net_spec),
        _print_weights(net_spec.print_weights),
    {

        create_net();

        //Work out number of total connections
        unsigned num_connections = 0;

        for(const auto& layer : _layers)
            num_connections += layer.get_number_of_connections();

        //If evolving weights as well as learning rates
        if(_net_spec.evolve_init_weights) 
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
            if(_net_spec.random_weight_init) 
            {

                UniformRealDistribution _uniform_distr(0., 1.);

                for(unsigned i = 0; i < num_connections; i++)
                    weights.push_back(_uniform_distr.next());

            } else 
            {

                for(unsigned i = 0; i < num_connections; i++)
                    weights.push_back(_net_spec.default_init_value);

            }

            propogate_learning_rates(traits);
            propogate_weights(weights);

        }

    }

    void propogate_learning_rates(const std::vector<double>& learning_rates) 
    {

        auto start = learning_rates.begin();
        auto end = learning_rates.begin();

        for(auto& layer : _layers)
        {

            end += layer.get_number_of_connections();

            const std::vector<double> tempW(start, end);
            layer.set_learning_rates(tempW);

            start += layer.get_number_of_connections();

        }

    }

protected:

    virtual HebbsNetwork* clone_impl() const override 
    { 
        return new HebbsNetwork(*this); 
    };

private:

    void create_net() override
    {
        for(unsigned i = 0; i < _net_spec.layer_specs.size(); i++)
            _layers.push_back(HebbsLayer(_net_spec.layer_specs.at(i), _trace));
    }

};

} // namespace NeuroEvo

#endif
