/*
    This example shows how a network solves the
    sequence classification task - a good task for
    testing a networks memory capability.
    This example also illustrates how you can alternatively
    build a network specification - layer by layer.
    It also illustrates the use fo GRUs in a network.
*/

#include <experiment.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <phenotype/phenotype_specs/layer_spec.h>
#include <domains/sequence_classification.h>
#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutator.h>
#include <optimiser/genetic_algorithm.h>
#include <util/maths/activation_functions/activation_function_specs/sigmoid_spec.h>
#include <gp_map/vector_to_network_map.h>

int main(int argc, const char* argv[]) 
{

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) 
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run: ./sequence_classification_example" << std::endl;
        std::cout << 
            "Individual run: ./sequence_classification_example *exp dir*/*population directory*"
            << std::endl;

        return -1;
    }

    typedef double gene_type;
    typedef double phenotype_output;

    //Build a network from layers
    //Build hidden layer
    const auto hl_layer_type = NeuroEvo::NeuronType::GRU;
    const unsigned hl_num_neurons = 3;
    const unsigned hl_inputs_per_neuron = 1;    // the number of inputs to the hidden layer
    const std::shared_ptr<NeuroEvo::ActivationFunctionSpec> hl_activation_spec(
        new NeuroEvo::SigmoidSpec()
    );
    NeuroEvo::LayerSpec hidden_layer(hl_num_neurons, hl_inputs_per_neuron, 
                                     hl_activation_spec, hl_layer_type);

    //Build output layer
    const auto ol_layer_type = NeuroEvo::NeuronType::Standard;
    const unsigned ol_num_neurons = 1;
    const unsigned ol_inputs_per_neuron = 3;    // the number of inputs to the output layer
    const std::shared_ptr<NeuroEvo::ActivationFunctionSpec> ol_activation_spec(
        new NeuroEvo::SigmoidSpec()
    );
    NeuroEvo::LayerSpec output_layer(ol_num_neurons, ol_inputs_per_neuron, 
                                     ol_activation_spec, ol_layer_type);

    // Build a network from the layer specifications
    std::vector<NeuroEvo::LayerSpec> layer_specs{hidden_layer, output_layer};

    auto pheno_spec = new NeuroEvo::NetworkBuilder(layer_specs);

    const unsigned num_genes = pheno_spec->get_num_params();

    // We have to define how the genotype vector is mapped to the network phenotype
    std::unique_ptr<NeuroEvo::GPMap<gene_type, phenotype_output>> gp_map(
        new NeuroEvo::VectorToNetworkMap(pheno_spec)
    );

    // Build sequence classification domain
    const unsigned depth = 3;           // the number of 1s and -1s
    const unsigned zeros_lower = 5;     // the smallest number of zeros interleaved in
    const unsigned zeros_upper = 5;     // the highest number of zeros interleaved in

    bool domain_trace = false;
    if(argc == 2) domain_trace = true;

    std::unique_ptr<NeuroEvo::Domain<gene_type, phenotype_output>> domain(
        new NeuroEvo::SequenceClassification<gene_type>(depth, zeros_upper,
                                                        zeros_lower, domain_trace)
    );

    // Construct experiment
    std::optional<NeuroEvo::Experiment<gene_type, phenotype_output>> experiment = 
        NeuroEvo::Experiment<gene_type, phenotype_output>::construct(
            *domain, 
            *gp_map
        );

    //Do not continue if experiment construction was not successful
    if(!experiment) exit(0);
    
    //Define genetic operators and parameters
    const unsigned pop_size = 150;
    const unsigned max_gens = 1000;
    
    // Specify the distribution used for the initial gene values
    const double init_gene_lower_bound = 0;
    const double init_gene_upper_bound = 1;
    std::unique_ptr<NeuroEvo::Distribution<gene_type>> genotype_distr(
        new NeuroEvo::UniformRealDistribution(init_gene_lower_bound, init_gene_upper_bound)
    );

    // Specify mutator
    const double mutation_rate = 0.4;
    const double mutation_power = 1.0;
    std::shared_ptr<NeuroEvo::Mutator<gene_type>> mutator(
        new NeuroEvo::RealGaussianMutator(mutation_rate, mutation_power)
    );

    // Selection mechanism
    std::unique_ptr<NeuroEvo::Selection<gene_type, phenotype_output>> selector(
        new NeuroEvo::RouletteWheelSelection<gene_type, phenotype_output>()
    );

    // Specify optimiser
    std::unique_ptr<NeuroEvo::Optimiser<gene_type, phenotype_output>> optimiser = 
        std::make_unique<NeuroEvo::GeneticAlgorithm<gene_type, phenotype_output>>(
       *selector,
       *mutator,
       *genotype_distr,
       *gp_map,
       num_genes,
       max_gens,
       pop_size
    );

    // Run either an evolutionary run or an individual run
    if(argc == 1) experiment->evolutionary_run(*optimiser);
    if(argc == 2) experiment->individual_run(argv[1]);

}
