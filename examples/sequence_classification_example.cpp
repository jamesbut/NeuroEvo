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
#include <util/random/uniform_real_distribution.h>
#include <util/maths/activation_functions/sigmoid.h>

int main(int argc, const char* argv[]) 
{

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) 
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run: ./sequence_classification_example" << std::endl;
        std::cout << "Individual run:   ./sequence_classification_example *population directory*"
                  << std::endl;

        return -1;
    }

    typedef double gene_type;

    //Build a network from layers
    //Build hidden layer
    const auto hl_layer_type = NeuroEvo::LayerSpec::NeuronType::GRU;
    const unsigned hl_num_neurons = 3;
    const unsigned hl_inputs_per_neuron = 1;    // the number of inputs to the hidden layer
    NeuroEvo::ActivationFunction* hl_activation_spec(new NeuroEvo::Sigmoid(1));
    NeuroEvo::LayerSpec hidden_layer(hl_layer_type, hl_num_neurons,
                                     hl_inputs_per_neuron, hl_activation_spec);

    //Build output layer
    const auto ol_layer_type = NeuroEvo::LayerSpec::NeuronType::Standard;
    const unsigned ol_num_neurons = 1;
    const unsigned ol_inputs_per_neuron = 3;    // the number of inputs to the output layer
    NeuroEvo::ActivationFunction* ol_activation_spec(new NeuroEvo::Sigmoid(1));
    NeuroEvo::LayerSpec output_layer(ol_layer_type, ol_num_neurons,
                                     ol_inputs_per_neuron, ol_activation_spec);

    // Build a network from the layer specifications
    std::vector<NeuroEvo::LayerSpec> layer_specs{hidden_layer, output_layer};

    const unsigned num_inputs = 1;
    std::unique_ptr<NeuroEvo::PhenotypeSpec<gene_type>> pheno_spec(
        new NeuroEvo::NetworkBuilder<gene_type>(num_inputs, layer_specs)
    );

    // Specify the distribution used for the initial gene values
    const double init_gene_lower_bound = 0;
    const double init_gene_upper_bound = 1;
    std::unique_ptr<NeuroEvo::Distribution<gene_type>> genotype_distr(
        new NeuroEvo::UniformRealDistribution(init_gene_lower_bound, init_gene_upper_bound)
    );

    // Specify genotype
    // This is done after specifying phenotype so the number
    // of genes required is known.
    std::unique_ptr<NeuroEvo::GenotypeSpec<gene_type>> geno_spec(
        new NeuroEvo::GenotypeSpec(pheno_spec->get_num_params(), *genotype_distr)
    );

    // Build sequence classification domain
    const unsigned depth = 3;           // the number of 1s and -1s
    const unsigned zeros_lower = 5;     // the smallest number of zeros interleaved in
    const unsigned zeros_upper = 5;     // the highest number of zeros interleaved in

    bool domain_trace = false;
    if(argc == 2) domain_trace = true;

    std::unique_ptr<NeuroEvo::Domain<gene_type>> domain(
        new NeuroEvo::SequenceClassification<gene_type>(depth, zeros_upper,
                                                        zeros_lower, domain_trace)
    );

    // Construct experiment
    std::optional<NeuroEvo::Experiment<gene_type>> experiment = 
        NeuroEvo::Experiment<gene_type>::construct(*domain, 
                                                   *geno_spec, 
                                                   *pheno_spec);

    //Do not continue if experiment construction was not successful
    if(!experiment) exit(0);
    
    //Define genetic operators and parameters
    const unsigned pop_size = 150;
    const unsigned max_gens = 1000;
    const double mutation_rate = 0.4;
    const double mutation_power = 1.0;

    std::unique_ptr<NeuroEvo::Mutator<gene_type>> mutator(
        new NeuroEvo::RealGaussianMutator(mutation_rate, mutation_power)
    );

    std::unique_ptr<NeuroEvo::Selection<gene_type>> selector(
        new NeuroEvo::RouletteWheelSelection<gene_type>()
    );

    // Run either an evolutionary run or an individual run
    if(argc == 1) experiment->evolutionary_run(pop_size, max_gens, selector.get());
    if(argc == 2) experiment->individual_run(argv[1]);

}
