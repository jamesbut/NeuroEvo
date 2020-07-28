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

class NetworkBuilder : public PhenotypeSpec
{

public:

    //Build with hidden layers all of the same size and type
    NetworkBuilder(const unsigned num_inputs, const unsigned num_outputs,
                   const unsigned num_hidden_layers, const unsigned neurons_per_layer,
                   const bool trace = false,
                   const std::shared_ptr<ActivationFunctionSpec> activation_func_spec = 
                       std::shared_ptr<ActivationFunctionSpec>(new SigmoidSpec()),
                   const NeuronType neuron_type = NeuronType::Standard);

    //Build with layer specs in which one can provde more fine grained detail
    NetworkBuilder(const std::vector<LayerSpec>& layer_specs,
                   const bool trace = false);

    //This function builds the network but it is the GPMaps responsibility to set
    //the weights from the genotype
    //One can provide initial weights to the network though
    Phenotype<double>* build_network(const std::optional<const std::vector<double>>& init_weights
                                     = std::nullopt) const;

    /* Builder functions */
    void make_recurrent();
    void add_layer(LayerSpec& layer_spec);
    void make_torch_net(const bool default_torch_net_init = true);
    void add_read_file(const std::string& file_path);
    void make_hebbian(const bool evolve_init_weights,
                      const std::optional<double> default_init_weight = std::nullopt,
                      const bool print_weights_to_file = false,
                      const std::optional<std::string>& weights_file_name = std::nullopt,
                      const std::optional<std::string>& outputs_file_name = std::nullopt);

    bool is_torch_net() const;
    const std::vector<LayerSpec>& get_layer_specs() const;
    bool get_trace() const;
    const HebbsSpec& get_hebbs_spec() const;
    unsigned get_num_inputs() const;
    unsigned get_num_outputs() const;

    auto clone() const;

protected:

    NetworkBuilder* clone_impl() const override;

private:

    unsigned required_num_genes(const std::vector<LayerSpec>& layer_specs);
    unsigned required_num_genes(
        const unsigned num_inputs,
        const unsigned num_outputs,
        const unsigned num_hidden_layers,
        const unsigned neurons_per_layer,
        const NeuronType neuron_type,
        const std::shared_ptr<ActivationFunctionSpec>& activation_func);

    const std::pair<std::vector<double>, std::vector<double>> 
        split_hebbs_traits(const std::vector<double>& traits) const;

    /* Essential parameters of any network */
    const unsigned _num_inputs;
    const unsigned _num_outputs;

    std::vector<LayerSpec> _layer_specs;

    //This serves 2 purposes: if this optional is not set then the network will not
    //be a Torch network.
    //If it is set then the value of the boolean will determine whether the Torch network
    //will be initialised according to the default torch settings or by the genotype
    //that is supplied.
    std::optional<bool> _default_torch_net_init;

    /* Optional parameters */
    std::optional<HebbsSpec> _hebbs_spec;

    std::optional<std::string> _read_file_path;

};

} // namespace NeuroEvo

#endif
