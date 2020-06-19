#ifndef _NETWORK_BUILDER_H_
#define _NETWORK_BUILDER_H_

/*
 * A NetworkBuilder is used to define and build different types of neural
 * networks.
 * For now networks can only be built from genotypes with genes of type double
 * although this can be made generic in the future.
*/

#include <memory>
#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <phenotype/phenotype_specs/layer_spec.h>
#include <util/maths/activation_functions/activation_function_specs/sigmoid_spec.h>
#include <phenotype/neural_network/network.h>
#include <phenotype/neural_network/hebbs_network.h>
#include <phenotype/phenotype_specs/hebbs_spec.h>
#include <phenotype/neural_network/torch_network.h>

namespace NeuroEvo {

class NetworkBuilder : public PhenotypeSpec<double, double> 
{

public:

    //Build with hidden layers all of the same size and type
    NetworkBuilder(const unsigned num_inputs, const unsigned num_outputs,
                   const unsigned num_hidden_layers, const unsigned neurons_per_layer,
                   const bool torch_net = false,
                   const LayerSpec::NeuronType neuron_type = LayerSpec::NeuronType::Standard,
                   const std::shared_ptr<ActivationFunctionSpec> activation_func = 
                       std::make_shared<ActivationFunctionSpec>(SigmoidSpec()),
                   const bool trace = false) :
        PhenotypeSpec<double, double>(required_num_genes(num_inputs, num_outputs, 
                                                         num_hidden_layers, neurons_per_layer, 
                                                         neuron_type, activation_func)),
        _num_inputs(num_inputs),
        _num_outputs(num_outputs),
        _layer_specs(LayerSpec::build_layer_specs(num_inputs, num_outputs, num_hidden_layers,
                                                  neurons_per_layer, neuron_type, 
                                                  activation_func)),
        _torch_net(torch_net),
        _trace(trace) {}

    //Build with layer specs in which one can provde more fine grained detail
    NetworkBuilder(const unsigned num_inputs, 
                   const std::vector<LayerSpec>& layer_specs,
                   const bool torch_net = false,
                   const bool trace = false) :
        PhenotypeSpec<double, double>(required_num_genes(num_inputs, layer_specs)),
        _num_inputs(num_inputs),
        _num_outputs(layer_specs.back().get_num_neurons()),
        _layer_specs(layer_specs),
        _torch_net(torch_net),
        _trace(trace) {}

    Phenotype<double>* generate_phenotype(Genotype<double>& genotype,
                                          GPMap<double, double>* gp_map) override 
    {

        //Check for GPMap
        if(gp_map) 
            return gp_map->map(genotype, *this);

        //Check for Hebbian
        if(_hebbs_spec)
        {

            HebbsNetwork* network = new HebbsNetwork(*_hebbs_spec, _trace);
            network->create_net(_layer_specs);

            //Split genes for Hebbs network into weights and learning rates
            const std::pair<std::vector<double>, std::vector<double>> split_genes =
                split_hebbs_traits(genotype.genes());
            network->propogate_weights(split_genes.first);
            network->propogate_learning_rates(split_genes.second); 

            return network;

        } else if (_torch_net)
        {
            
            TorchNetwork* torch_network = new TorchNetwork(_layer_specs, 
                                                           genotype.genes(), 
                                                           _trace);
            return torch_network;

        } else
        {
        
            Network* network = new Network(_trace);
            network->create_net(_layer_specs);
            network->propogate_weights(genotype.genes());

            return network;

        }
        
    }

    auto clone() const 
    { 
        return std::unique_ptr<NetworkBuilder>(clone_impl()); 
    }

    /* Builder functions */
    void make_recurrent() 
    {
        //TODO: rebuild layer specs!
        std::cerr << "Rebuild layer specs in network builder!" << std::endl;
        exit(0);
    }

    void add_layer(LayerSpec& layer_spec) 
    {
        //TODO: Write!
        std::cerr << "Write add_layer in NetworkBuilder" << std::endl;
        exit(0);
    }

    void make_torch_net()
    {
        _torch_net = true;
    }

    const std::vector<LayerSpec>& get_layer_specs() const
    {
        return _layer_specs;
    }

    bool get_trace() const 
    {
        return _trace;
    }

public:

    void make_hebbian(const bool evolve_init_weights,
                      const std::optional<double> default_init_weight = std::nullopt,
                      const bool print_weights_to_file = false,
                      const std::optional<std::string>& weights_file_name = std::nullopt,
                      const std::optional<std::string>& outputs_file_name = std::nullopt)
    {

        if(evolve_init_weights)
            _hebbs_spec.emplace(HebbsSpec(true, std::nullopt, print_weights_to_file,
                                          weights_file_name, outputs_file_name));
        else
            _hebbs_spec.emplace(HebbsSpec(false, default_init_weight, print_weights_to_file,
                                          weights_file_name, outputs_file_name));
        
        //Alter number of params
        this->_num_params = required_num_genes(_num_inputs, _layer_specs);

    }

    const HebbsSpec& get_hebbs_spec() const 
    {
        return *_hebbs_spec;
    }

    unsigned get_num_inputs() const
    {
        return _num_inputs;
    }

    unsigned get_num_outputs() const
    {
        return _num_outputs;
    }

protected:

    NetworkBuilder* clone_impl() const override
    {
        return new NetworkBuilder(*this);
    } 

private:

    unsigned required_num_genes(const unsigned num_inputs,
                                const std::vector<LayerSpec>& layer_specs)
    {

        unsigned num_genes = 0;

        for(const auto& layer_spec : layer_specs)
            num_genes += layer_spec.get_num_neurons() * layer_spec.get_params_per_neuron();

        //If the specification is Hebbs, double the number of genes if the initial weights
        //are being evolved
        if(_hebbs_spec)
            if(_hebbs_spec->get_evolve_init_weights())
                num_genes *= 2;

        return num_genes;

    }

    unsigned required_num_genes(const unsigned num_inputs,
                                const unsigned num_outputs,
                                const unsigned num_hidden_layers,
                                const unsigned neurons_per_layer,
                                const LayerSpec::NeuronType neuron_type,
                                const std::shared_ptr<ActivationFunctionSpec>& activation_func) 
    {

        //Build LayerSpecs from the specification
        std::vector<LayerSpec> layer_specs = LayerSpec::build_layer_specs(num_inputs, num_outputs,
                                                                          num_hidden_layers, 
                                                                          neurons_per_layer,
                                                                          neuron_type, 
                                                                          activation_func);
        return required_num_genes(num_inputs, layer_specs);

    } 

    const std::pair<std::vector<double>, std::vector<double>> 
        split_hebbs_traits(const std::vector<double>& traits)
    {
        //Work out number of total connections
        unsigned num_connections = 0;

        for(const auto& layer_spec : _layer_specs)
            num_connections += layer_spec.get_num_weights();

        //If evolving weights as well as learning rates
        if(_hebbs_spec->get_evolve_init_weights()) 
        {

            //Split genes into 2 - learning rates and initial weights
            const std::vector<double> learning_rates(traits.begin(),
                                                     traits.begin() + num_connections);

            const std::vector<double> weights(traits.begin() + num_connections,
                                              traits.end());

            return std::make_pair(weights, learning_rates);

        } else   //If just evolving learning rates
        {    

            std::vector<double> weights;
            weights.reserve(num_connections);

            //If the weights are randomly initialised or all set to some value
            if(!_hebbs_spec->get_default_weight_init()) 
            {

                UniformRealDistribution _uniform_distr(0., 1.);

                for(unsigned i = 0; i < num_connections; i++)
                    weights.push_back(_uniform_distr.next());

            } else 
            {
                for(unsigned i = 0; i < num_connections; i++)
                    weights.push_back(*_hebbs_spec->get_default_weight_init());
            }

            return std::make_pair(weights, traits);

        }

    }

    /* Essential parameters of any network */
    const unsigned _num_inputs;
    const unsigned _num_outputs;

    std::vector<LayerSpec> _layer_specs;

    bool _torch_net;

    const bool _trace;

    /* Optional parameters */
    std::optional<HebbsSpec> _hebbs_spec;

};

} // namespace NeuroEvo

#endif
