#include <phenotype/phenotype_specs/network_builder.h>
#include <util/exceptions/not_implemented_exception.h>

namespace NeuroEvo {

NetworkBuilder::NetworkBuilder(const unsigned num_inputs,
                               const unsigned num_outputs,
                               const unsigned num_hidden_layers,
                               const unsigned neurons_per_layer,
                               const std::shared_ptr<ActivationFunctionSpec>
                                   hl_activation_func_spec,
                               const std::shared_ptr<ActivationFunctionSpec>
                                   ol_activation_func_spec,
                               const bool batch_norm,
                               const bool bias,
                               const NeuronType neuron_type,
                               const bool trace) :
    PhenotypeSpec(required_num_genes(num_inputs, num_outputs,
                                     num_hidden_layers, neurons_per_layer,
                                     neuron_type, hl_activation_func_spec,
                                     ol_activation_func_spec, batch_norm, bias),
                  trace),
    _num_inputs(num_inputs),
    _num_outputs(num_outputs),
    _layer_specs(LayerSpec::build_layer_specs(num_inputs, num_outputs,
                                              num_hidden_layers,
                                              neurons_per_layer, neuron_type,
                                              hl_activation_func_spec,
                                              ol_activation_func_spec,
                                              batch_norm,
                                              bias)),
    _torch_net(false) {}

NetworkBuilder::NetworkBuilder(const unsigned num_inputs,
                               const unsigned num_outputs,
                               const unsigned num_hidden_layers,
                               const unsigned neurons_per_layer,
                               const std::shared_ptr<ActivationFunctionSpec>
                                   activation_func_spec,
                               const bool batch_norm,
                               const bool bias,
                               const NeuronType neuron_type,
                               const bool trace) :
    NetworkBuilder(num_inputs, num_outputs, num_hidden_layers, neurons_per_layer,
                   activation_func_spec, activation_func_spec, batch_norm, bias,
                   neuron_type, trace) {}

NetworkBuilder::NetworkBuilder(const std::vector<LayerSpec>& layer_specs,
                               const bool trace,
                               const bool torch_net,
                               const std::optional<const std::string>& read_file) :
    PhenotypeSpec(required_num_genes(layer_specs), trace),
    _num_inputs(layer_specs.front().get_inputs_per_neuron()),
    _num_outputs(layer_specs.back().get_num_neurons()),
    _layer_specs(layer_specs),
    _torch_net(torch_net),
    _read_file_path(read_file) {}

NetworkBuilder::NetworkBuilder(const JSON& json) :
    NetworkBuilder(json.has_value({"file_path"}) ?
                       TorchNetwork::read_layer_specs(
                           std::string(NEURO_EVO_CMAKE_SRC_DIR) +
                           json.get<std::string>({"file_path"})) :
                       LayerSpec::build_layer_specs(json),
                   json.value({"trace"}, false),
                   json.value({"torch_net"}, false),
                   json.has_value({"file_path"}) ?
                       std::make_optional(
                           std::string(NEURO_EVO_CMAKE_SRC_DIR) +
                           json.get<std::string>({"file_path"})
                       ) :
                       std::nullopt)
    {
        if(json.has_value({"weights"}))
            set_init_weights(json.at({"weights"}));
    }

NetworkBuilder::NetworkBuilder(const NetworkBuilder& network_builder) :
    PhenotypeSpec(required_num_genes(network_builder._layer_specs,
                                     network_builder._trace)),
    _num_inputs(network_builder._num_inputs),
    _num_outputs(network_builder._num_outputs),
    _layer_specs(network_builder._layer_specs),
    _torch_net(network_builder._torch_net),
    _init_weights(network_builder._init_weights),
    _init_weight_distr(network_builder._init_weight_distr ?
                       network_builder._init_weight_distr->clone() :
                       nullptr),
    _hebbs_spec(network_builder._hebbs_spec),
    _read_file_path(network_builder._read_file_path) {}

Phenotype<double>* NetworkBuilder::build_network()
{

    //Generate init weights if a distribution has been given
    if(_init_weight_distr)
        _init_weights = generate_init_weights();

    //Check init weights size if needed
    if(_init_weights.has_value())
        if(_init_weights->size() != get_num_params())
            throw std::length_error(
                "The number of genes given to build the network was not equal"
                " to the number of params required by the network\n"
                "Num genes: " + std::to_string(_init_weights->size()) +
                "\nNum params required: " + std::to_string(get_num_params()));

    //Check for Hebbian
    if(_hebbs_spec)
    {

        HebbsNetwork* network = new HebbsNetwork(*_hebbs_spec, _trace);
        network->create_net(_layer_specs);

        //Split weights for Hebbs network into weights and learning rates
        const std::pair<std::vector<double>, std::vector<double>> split_weights =
            split_hebbs_traits(_init_weights.value());
        network->propogate_weights(split_weights.first);
        network->propogate_learning_rates(split_weights.second);

        return network;

    }
#if USE_TORCH
    else if (_torch_net)
    {
        TorchNetwork* torch_network;

        //If the torch network is read from file
        if(_read_file_path)
            torch_network = new TorchNetwork(_read_file_path.value(),
                                             _trace);
        else
            torch_network = new TorchNetwork(_layer_specs,
                                             _init_weights,
                                             _trace);

        return torch_network;

    }
#endif
    else
    {
        Network* network = new Network(_trace);
        network->create_net(_layer_specs);

        if(_init_weights)
            network->propogate_weights(_init_weights.value());

        return network;
    }

}

void NetworkBuilder::make_recurrent()
{
    throw NotImplementedException("NetworkBuilder::make_recurrent()");
}

void NetworkBuilder::add_layer(LayerSpec& layer_spec)
{
    throw NotImplementedException("NetworkBuilder::add_layer(LayerSpec&)");
}

void NetworkBuilder::make_torch_net(const bool torch_net)
{
    _torch_net = torch_net;
}

void NetworkBuilder::add_read_file(const std::string& file_path)
{
    _read_file_path = file_path;
}

void NetworkBuilder::make_hebbian(
    const bool evolve_init_weights,
    const std::optional<double> default_init_weight,
    const bool print_weights_to_file,
    const std::optional<std::string>& weights_file_name,
    const std::optional<std::string>& outputs_file_name
)
{

    if(evolve_init_weights)
        _hebbs_spec.emplace(HebbsSpec(true, std::nullopt, print_weights_to_file,
                                      weights_file_name, outputs_file_name));
    else
        _hebbs_spec.emplace(HebbsSpec(false, default_init_weight,
                                      print_weights_to_file,
                                      weights_file_name, outputs_file_name));

    //Alter number of params
    this->_num_params = required_num_genes(_layer_specs, _hebbs_spec);

}

void NetworkBuilder::set_init_weights(const std::vector<double>& init_weights)
{
    _init_weights = init_weights;
}

void NetworkBuilder::set_init_weight_distribution(
    Distribution<double>* init_weight_distr)
{
    _init_weight_distr.reset(init_weight_distr);
}

bool NetworkBuilder::is_torch_net() const
{
    return _torch_net;
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

unsigned NetworkBuilder::required_num_genes(
    const std::vector<LayerSpec>& layer_specs,
    const std::optional<HebbsSpec>& hebbs_spec
)
{

    unsigned num_genes = 0;

    for(const auto& layer_spec : layer_specs)
        num_genes += layer_spec.get_num_neurons() *
                     layer_spec.get_params_per_neuron();

    //If the specification is Hebbs, double the number of genes if the initial
    //weights are being evolved
    if(hebbs_spec)
        if(hebbs_spec->get_evolve_init_weights())
            num_genes *= 2;

    return num_genes;

}

unsigned NetworkBuilder::required_num_genes(
    const unsigned num_inputs,
    const unsigned num_outputs,
    const unsigned num_hidden_layers,
    const unsigned neurons_per_layer,
    const NeuronType neuron_type,
    const std::shared_ptr<ActivationFunctionSpec>& hl_activation_func,
    const std::shared_ptr<ActivationFunctionSpec>& ol_activation_func,
    const bool batch_norm,
    const bool bias)
{

    //Build LayerSpecs from the specification
    std::vector<LayerSpec> layer_specs = LayerSpec::build_layer_specs(
        num_inputs,
        num_outputs,
        num_hidden_layers,
        neurons_per_layer,
        neuron_type,
        hl_activation_func,
        ol_activation_func,
        batch_norm,
        bias);
    return required_num_genes(layer_specs);

}

unsigned NetworkBuilder::required_num_genes(
    const unsigned num_inputs,
    const unsigned num_outputs,
    const unsigned num_hidden_layers,
    const unsigned neurons_per_layer,
    const NeuronType neuron_type,
    const std::shared_ptr<ActivationFunctionSpec>& activation_func,
    const bool batch_norm,
    const bool bias)
{

    //Build LayerSpecs from the specification
    std::vector<LayerSpec> layer_specs = LayerSpec::build_layer_specs(
        num_inputs,
        num_outputs,
        num_hidden_layers,
        neurons_per_layer,
        neuron_type,
        activation_func,
        bias
    );
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

const std::vector<double> NetworkBuilder::generate_init_weights() const
{
    std::vector<double> init_weights(get_num_params());
    for(std::size_t i = 0; i < init_weights.size(); i++)
        init_weights[i] = _init_weight_distr->next();
    return init_weights;
}

auto NetworkBuilder::clone() const
{
    return std::unique_ptr<NetworkBuilder>(clone_impl());
}

JSON NetworkBuilder::to_json_impl() const
{
    JSON json;
    json.emplace("name", "NetworkBuilder");
    json.emplace("num_inputs", _num_inputs);
    json.emplace("num_outputs", _num_outputs);
    json.emplace("torch_net", _torch_net);
    if(_hebbs_spec.has_value())
        json.emplace("hebbs_spec", _hebbs_spec->to_json().at());
    if(_read_file_path.has_value())
        json.emplace("read_file_path", _read_file_path.value());
    for(std::size_t i = 0; i < _layer_specs.size(); i++)
        json.emplace("LayerSpec" + std::to_string(i),
                     _layer_specs.at(i).to_json().at());
    return json;
}

NetworkBuilder* NetworkBuilder::clone_impl() const
{
    return new NetworkBuilder(*this);
}

} // namespace NeuroEvo
