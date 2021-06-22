#include "phenotype/neural_network/hebbs_layer.h"
#include <phenotype/phenotype_specs/layer_spec.h>

namespace NeuroEvo {

LayerSpec::LayerSpec(const unsigned num_neurons,
                     const unsigned inputs_per_neuron,
                     const std::shared_ptr<ActivationFunctionSpec> activation_func_spec,
                     const bool batch_norm,
                     const NeuronType neuron_type,
                     const bool bias) :
    _neuron_type(neuron_type),
    _num_neurons(num_neurons),
    _inputs_per_neuron(inputs_per_neuron),
    _activation_func_spec(activation_func_spec),
    _batch_norm(batch_norm),
    _bias(bias)
{

    //Normal
    if(_neuron_type == Standard)
    {
        _params_per_neuron = inputs_per_neuron;
        if(_bias)
            _params_per_neuron += 1;
    }

    //Recurrent
    if(_neuron_type == Recurrent)
    {
        _params_per_neuron = inputs_per_neuron + 1;
        if(_bias)
            _params_per_neuron += 1;
    }

    //GRU
    if(_neuron_type == GRU)
        _params_per_neuron = (inputs_per_neuron * 3) + 6;

}

LayerSpec::LayerSpec(const LayerSpec& layer_spec) :
    _neuron_type(layer_spec._neuron_type),
    _num_neurons(layer_spec._num_neurons),
    _inputs_per_neuron(layer_spec._inputs_per_neuron),
    _params_per_neuron(layer_spec._params_per_neuron),
    _activation_func_spec(layer_spec._activation_func_spec),
    _batch_norm(layer_spec._batch_norm),
    _bias(layer_spec._bias) {}

LayerSpec& LayerSpec::operator=(const LayerSpec& layer_spec)
{
    _neuron_type = layer_spec._neuron_type;
    _num_neurons = layer_spec._num_neurons;
    _inputs_per_neuron = layer_spec._inputs_per_neuron;
    _params_per_neuron = layer_spec._params_per_neuron;
    _activation_func_spec = layer_spec._activation_func_spec;
    _batch_norm = layer_spec._batch_norm;
    _bias = layer_spec._bias;

    return *this;
}

std::vector<LayerSpec> LayerSpec::build_layer_specs(
        const unsigned num_inputs,
        const unsigned num_outputs,
        const unsigned num_hidden_layers,
        const unsigned neurons_per_layer,
        const NeuronType neuron_type,
        const std::shared_ptr<ActivationFunctionSpec>& activation_func_spec,
        const bool batch_norm,
        const bool bias)
{
    return build_layer_specs(num_inputs, num_outputs, num_hidden_layers,
                             neurons_per_layer, neuron_type, activation_func_spec,
                             activation_func_spec, batch_norm, bias);
}

std::vector<LayerSpec> LayerSpec::build_layer_specs(
        const unsigned num_inputs,
        const unsigned num_outputs,
        const unsigned num_hidden_layers,
        const unsigned neurons_per_layer,
        const NeuronType neuron_type,
        const std::shared_ptr<ActivationFunctionSpec>& hl_activation_func_spec,
        const std::shared_ptr<ActivationFunctionSpec>& ol_activation_func_spec,
        const bool batch_norm,
        const bool bias)
{

    std::vector<LayerSpec> layer_specs;

    if(num_hidden_layers == 0)
        layer_specs.push_back(LayerSpec(num_outputs, num_inputs,
                                        ol_activation_func_spec, batch_norm,
                                        neuron_type, bias));
    else
    {

        layer_specs.push_back(LayerSpec(neurons_per_layer, num_inputs,
                                        hl_activation_func_spec, batch_norm,
                                        neuron_type, bias));

        for(unsigned i = 1; i < num_hidden_layers; i++)
            layer_specs.push_back(LayerSpec(neurons_per_layer, neurons_per_layer,
                                            hl_activation_func_spec, batch_norm,
                                            neuron_type, bias));

        layer_specs.push_back(LayerSpec(num_outputs, neurons_per_layer,
                                        ol_activation_func_spec, batch_norm,
                                        neuron_type, bias));

    }

    return layer_specs;

}

NeuronType LayerSpec::get_neuron_type() const
{
    return _neuron_type;
}

unsigned LayerSpec::get_num_neurons() const
{
    return _num_neurons;
}

unsigned LayerSpec::get_inputs_per_neuron() const
{
    return _inputs_per_neuron;
}

unsigned LayerSpec::get_params_per_neuron() const
{
    return _params_per_neuron;
}

unsigned LayerSpec::get_num_weights() const
{
    return _num_neurons * _params_per_neuron;
}

const std::shared_ptr<ActivationFunctionSpec>& LayerSpec::get_activation_func_spec()
    const
{
    return _activation_func_spec;
}

bool LayerSpec::get_batch_norm() const
{
    return _batch_norm;
}

bool LayerSpec::get_bias() const
{
    return _bias;
}

void LayerSpec::print_spec() const
{
    std::cout << "Layer Spec:" << std::endl;
    std::cout << "Neuron type: " << _neuron_type << std::endl;
    std::cout << "Num neurons: " << _num_neurons << std::endl;
    std::cout << "Inputs per neuron: " << _inputs_per_neuron << std::endl;
    std::cout << "Params per neuron: " << _params_per_neuron << std::endl;
    std::cout << "Bias: " << _bias << std::endl;
}

} // namespace NeuroEvo
