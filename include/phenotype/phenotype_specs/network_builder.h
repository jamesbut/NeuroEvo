#ifndef _NETWORK_BUILDER_H_
#define _NETWORK_BUILDER_H_

/*
 * A NetworkBuilder is used to define and build different types of neural
 * networks.
*/

#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <phenotype/phenotype_specs/layer_spec.h>
#include <util/maths/activation_functions/sigmoid.h>

namespace NeuroEvo {

template <typename G>
class NetworkBuilder : public PhenotypeSpec<G> 
{

public:

    //Build with hidden layers all of the same size and type
    NetworkBuilder(const unsigned num_inputs, const unsigned num_outputs,
                   const unsigned num_hidden_layers, const unsigned neurons_per_layer,
                   const LayerSpec::NeuronType neuron_type = LayerSpec::NeuronType::Standard,
                   const ActivationFunction* activation_func = new Sigmoid()) :
        _num_inputs(num_inputs),
        _num_outputs(num_outputs),
        _layer_specs(build_layer_specs(num_inputs, num_outputs, num_hidden_layers,
                                       neurons_per_layer, neuron_type, activation_func)),
        PhenotypeSpec<G>(required_num_genes(num_inputs, num_outputs, num_hidden_layers,
                                            neurons_per_layer, neuron_type)) {}

    //Build with layer specs in which one can provde more fine grained detail
    NetworkBuilder(const unsigned num_inputs, 
                   const std::vector<LayerSpec>& layer_specs) :
        _num_inputs(num_inputs),
        _num_outputs(layer_specs.back().get_num_neurons()),
        _layer_specs(layer_specs),
        PhenotypeSpec<G>(required_num_genes(num_inputs, layer_specs)) {}

    Phenotype* generate_phenotype(Genotype<G>& genotype,
                                  GPMap<G>* gp_map) override {

        std::cerr << "Write how to generate network in NetworkBuilder!" << std::endl;
        exit(0);
        return nullptr;

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

private:    
    struct HebbsSpec;

public:

    void make_hebbian(const bool evolve_init_weights,
                      const std::optional<double> default_init_weight = std::nullopt)
    {

        if(evolve_init_weights)
            _hebbs_spec = HebbsSpec(true);
        else
            _hebbs_spec = HebbsSpec(false, default_init_weight);
        
        //Alter number of params
        this->_num_params = required_num_genes(_num_inputs, _layer_specs);

    }

protected:

    NetworkBuilder* clone_impl() const
    {
        return new NetworkBuilder(*this);
    } 

    const unsigned get_num_outputs(const std::vector<LayerSpec>& layer_specs) const 
    {
        return layer_specs.back().get_num_neurons();
    }

private:

    std::vector<LayerSpec> build_layer_specs(const unsigned num_inputs,
                                             const unsigned num_outputs,
                                             const unsigned num_hidden_layers,
                                             const unsigned neurons_per_layer,
                                             const LayerSpec::NeuronType neuron_type,
                                             ActivationFunction* activation_func) 
    {

        std::vector<LayerSpec> layer_specs;

        if(num_hidden_layers == 0)
            layer_specs.push_back(LayerSpec(neuron_type, num_outputs, 
                                            num_inputs, activation_func));
        else 
        {

            layer_specs.push_back(LayerSpec(neuron_type, neurons_per_layer, 
                                            num_inputs, activation_func));

            for(unsigned i = 1; i < num_hidden_layers; i++)
                layer_specs.push_back(LayerSpec(neuron_type, neurons_per_layer, 
                                                neurons_per_layer, activation_func));

            layer_specs.push_back(LayerSpec(neuron_type, num_outputs, 
                                            neurons_per_layer, activation_func));

        }

        return layer_specs;

    }

    const unsigned required_num_genes(const unsigned num_inputs,
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

    const unsigned required_num_genes(const unsigned num_inputs,
                                      const unsigned num_outputs,
                                      const unsigned num_hidden_layers,
                                      const unsigned neurons_per_layer,
                                      const LayerSpec::NeuronType neuron_type,
                                      const ActivationFunction* activation_func) 
    {

        //Build LayerSpecs from the specification
        std::vector<LayerSpec> layer_specs = build_layer_specs(num_inputs, num_outputs,
                                                               num_hidden_layers, 
                                                               neurons_per_layer,
                                                               neuron_type, activation_func);
        return required_num_genes(num_inputs, layer_specs);

    } 

    /* Contains information about a Hebbian specification. 
     * A Hebbian network can either evolve the initial weights before hebbian training
     * or it can not evolve them - if it doesn't evolve them, the question is whether
     * the intial weights are set randomly or to a some default value.
     */
    struct HebbsSpec
    {

    public:

        HebbsSpec(const bool evolve_init_weights,
                  const std::optional<double> default_init_weight = std::nullopt) :
            _evolve_init_weights(evolve_init_weights),
            _default_init_weight(default_init_weight) {}

        const bool get_evolve_init_weights() const 
        {
            return _evolve_init_weights;
        }

    private:

        const bool _evolve_init_weights;
        const std::optional<double> _default_init_weight;

    };

    /* Essential parameters of any network */
    const unsigned _num_inputs;
    const unsigned _num_outputs;

    std::vector<LayerSpec> _layer_specs;

    /* Optional parameters */
    std::optional<HebbsSpec> _hebbs_spec;

};

} // namespace NeuroEvo

#endif
