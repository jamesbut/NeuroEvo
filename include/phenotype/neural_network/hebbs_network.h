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

    HebbsNetwork(const HebbsSpec& hebbs_spec,
                 const bool trace = false) :
        Network(trace),
        _hebbs_spec(hebbs_spec) {}

    void create_net(const std::vector<LayerSpec>& layer_specs) override
    {
        for(const auto& layer_spec : layer_specs)
            _layers.push_back(
                std::unique_ptr<HebbsLayer>(new HebbsLayer(layer_spec, _trace))
            );

        for(const auto& layer : _layers)
            layer->create_layer();
    }

    void propogate_learning_rates(const std::vector<double>& learning_rates) override
    {
        auto start = learning_rates.begin();
        auto end = learning_rates.begin();

        for(auto& layer : _layers)
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
