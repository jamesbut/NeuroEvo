#include <phenotype/phenotype_specs/network_builder.h>

namespace NeuroEvo {

NetworkBuilder::NetworkBuilder(const unsigned num_inputs, const unsigned num_outputs,
                               const unsigned num_hidden_layers, const unsigned neurons_per_layer,
                               const bool trace,
                               const std::shared_ptr<ActivationFunctionSpec> activation_func_spec,
                               const NeuronType neuron_type) :
    PhenotypeSpec(required_num_genes(num_inputs, num_outputs, 
                                     num_hidden_layers, neurons_per_layer, 
                                     neuron_type, activation_func_spec)),
    _num_inputs(num_inputs),
    _num_outputs(num_outputs),
    _layer_specs(LayerSpec::build_layer_specs(num_inputs, num_outputs, num_hidden_layers,
                                              neurons_per_layer, neuron_type, 
                                              activation_func_spec)),
    _trace(trace) {}

NetworkBuilder::NetworkBuilder(const std::vector<LayerSpec>& layer_specs,
                               const bool trace) :
    PhenotypeSpec(required_num_genes(layer_specs)),
    _num_inputs(layer_specs.front().get_inputs_per_neuron()),
    _num_outputs(layer_specs.back().get_num_neurons()),
    _layer_specs(layer_specs),
    _trace(trace) {}

Phenotype<double>* NetworkBuilder::build_network(
        const std::optional<const std::vector<double>>& init_weights) const
{
    //Check for Hebbian
    if(_hebbs_spec)
    {

        HebbsNetwork* network = new HebbsNetwork(*_hebbs_spec, _trace);
        network->create_net(_layer_specs);

        //Split weights for Hebbs network into weights and learning rates
        const std::pair<std::vector<double>, std::vector<double>> split_weights =
            split_hebbs_traits(init_weights.value());
        network->propogate_weights(split_weights.first);
        network->propogate_learning_rates(split_weights.second); 

        return network;

    //If it is a torch network
    } else if (_default_torch_net_init)
    {

        TorchNetwork* torch_network;
        
        //If weights are not given
        //if(_default_torch_net_init.value())
        if(!init_weights)
            //If the torch network is read from file
            if(_read_file_path)
                torch_network = new TorchNetwork(_read_file_path.value(), 
                                                 _layer_specs,
                                                 _trace);
            else
                torch_network = new TorchNetwork(_layer_specs, 
                                                 _trace);
        //If torch network is initialised by given weights
        else 
            torch_network = new TorchNetwork(_layer_specs, 
                                             init_weights.value(), 
                                             _trace);
        return torch_network;

    } else
    {
    
        Network* network = new Network(_trace);
        network->create_net(_layer_specs);

        if(init_weights)
            network->propogate_weights(init_weights.value());

        return network;

    }
    
}

void NetworkBuilder::make_recurrent() 
{
    //TODO: rebuild layer specs!
    std::cerr << "Rebuild layer specs in network builder!" << std::endl;
    exit(0);
}

void NetworkBuilder::add_layer(LayerSpec& layer_spec) 
{
    //TODO: Write!
    std::cerr << "Write add_layer in NetworkBuilder" << std::endl;
    exit(0);
}

void NetworkBuilder::make_torch_net(const bool default_torch_net_init)
{
    _default_torch_net_init = std::optional<bool>(default_torch_net_init);
}

void NetworkBuilder::add_read_file(const std::string& file_path) 
{
    _read_file_path = file_path;
}

void NetworkBuilder::make_hebbian(const bool evolve_init_weights,
                                  const std::optional<double> default_init_weight,
                                  const bool print_weights_to_file,
                                  const std::optional<std::string>& weights_file_name,
                                  const std::optional<std::string>& outputs_file_name)
{

    if(evolve_init_weights)
        _hebbs_spec.emplace(HebbsSpec(true, std::nullopt, print_weights_to_file,
                                        weights_file_name, outputs_file_name));
    else
        _hebbs_spec.emplace(HebbsSpec(false, default_init_weight, print_weights_to_file,
                                        weights_file_name, outputs_file_name));
    
    //Alter number of params
    this->_num_params = required_num_genes(_layer_specs);

}

void NetworkBuilder::set_trace(const bool trace)
{
    _trace = trace;
}

bool NetworkBuilder::is_torch_net() const
{
    return _default_torch_net_init.has_value();
}

const std::vector<LayerSpec>& NetworkBuilder::get_layer_specs() const
{
    return _layer_specs;
}

bool NetworkBuilder::get_trace() const 
{
    return _trace;
}

const HebbsSpec& NetworkBuilder::get_hebbs_spec() const 
{
    return *_hebbs_spec;
}

unsigned NetworkBuilder::get_num_inputs() const
{
    return _num_inputs;
}

unsigned NetworkBuilder::get_num_outputs() const
{
    return _num_outputs;
}

auto NetworkBuilder::clone() const 
{ 
    return std::unique_ptr<NetworkBuilder>(clone_impl()); 
}

NetworkBuilder* NetworkBuilder::clone_impl() const
{
    return new NetworkBuilder(*this);
} 

unsigned NetworkBuilder::required_num_genes(const std::vector<LayerSpec>& layer_specs)
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

unsigned NetworkBuilder::required_num_genes(
    const unsigned num_inputs,
    const unsigned num_outputs,
    const unsigned num_hidden_layers,
    const unsigned neurons_per_layer,
    const NeuronType neuron_type,
    const std::shared_ptr<ActivationFunctionSpec>& activation_func) 
{

    //Build LayerSpecs from the specification
    std::vector<LayerSpec> layer_specs = LayerSpec::build_layer_specs(num_inputs, 
                                                                        num_outputs,
                                                                        num_hidden_layers, 
                                                                        neurons_per_layer,
                                                                        neuron_type, 
                                                                        activation_func);
    return required_num_genes(layer_specs);

} 

const std::pair<std::vector<double>, std::vector<double>> 
    NetworkBuilder::split_hebbs_traits(const std::vector<double>& traits) const
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

} // namespace NeuroEvo
