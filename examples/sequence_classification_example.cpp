/*
    This example shows how a network solves the
    sequence classification task - a good task for
    testing a networks memory capability.
    This example also illustrates how you can alternatively
    build a network specification - layer by layer.
    It also illustrates the use fo GRUs in a network.
*/

#include <experiment.h>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <phenotype/phenotype_specs/layer_spec.h>
#include <domains/sequence_classification.h>
#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutation.h>
#include <util/random/uniform_distribution.h>
#include <util/maths/activation_functions/sigmoid.h>

int main(int argc, const char* argv[]) {

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run: ./sequence_classification_example" << std::endl;
        std::cout << "Individual run:   ./sequence_classification_example *population directory*"
                  << std::endl;

        return -1;
    }

    //Build a network from layers
    /*
        Layer types:
        0: Standard
        1: Recurrent
        2: GRU
    */
    const unsigned HL_LAYER_TYPE = 2;
    const unsigned HL_NUM_NEURONS = 3;
    const unsigned HL_INPUTS_PER_NEURON = 1;    // The number of inputs to the hidden layer
    NeuroEvo::Utils::ActivationFunction* hl_activation_spec(new NeuroEvo::Utils::Sigmoid(1));
    NeuroEvo::Phenotypes::LayerSpec hidden_layer(HL_LAYER_TYPE, HL_NUM_NEURONS,
                                                 HL_INPUTS_PER_NEURON, hl_activation_spec);

    const unsigned OL_LAYER_TYPE = 0;
    const unsigned OL_NUM_NEURONS = 1;
    const unsigned OL_INPUTS_PER_NEURON = 3;    // The number of inputs to the output layer
    NeuroEvo::Utils::ActivationFunction* ol_activation_spec(new NeuroEvo::Utils::Sigmoid(1));
    NeuroEvo::Phenotypes::LayerSpec output_layer(OL_LAYER_TYPE, OL_NUM_NEURONS,
                                                 OL_INPUTS_PER_NEURON, ol_activation_spec);

    // Build a network from the layer specifications
    std::vector<NeuroEvo::Phenotypes::LayerSpec> layer_specs{hidden_layer, output_layer};

    const unsigned NUM_INPUTS = 1;
    std::unique_ptr<NeuroEvo::Phenotypes::PhenotypeSpec> pheno_spec(
        new NeuroEvo::Phenotypes::FixedNetworkSpec(NUM_INPUTS, layer_specs)
    );

    // Specify the distribution used for the initial gene values
    const double INIT_GENE_LOWER_BOUND = 0;
    const double INIT_GENE_UPPER_BOUND = 1;
    std::unique_ptr<NeuroEvo::Utils::Distribution> genotype_distr(
        new NeuroEvo::Utils::UniformDistribution(INIT_GENE_LOWER_BOUND, INIT_GENE_UPPER_BOUND)
    );

    // Specify genotype
    // This is done after specifying phenotype so the number
    // of genes required is known.
    std::unique_ptr<NeuroEvo::Genotypes::GenotypeSpec> geno_spec(
        new NeuroEvo::Genotypes::GenotypeSpec(pheno_spec->get_num_params(), *genotype_distr)
    );

    // Build sequence classification domain
    const unsigned DEPTH = 3;           // The number of 1s and -1s
    const unsigned ZEROS_LOWER = 5;     // The smallest number of zeros interleaved in
    const unsigned ZEROS_UPPER = 5;     // The highest number of zeros interleaved in

    bool DOMAIN_TRACE = false;
    if(argc == 2) DOMAIN_TRACE = true;

    std::unique_ptr<NeuroEvo::Domains::Domain> domain(
        new NeuroEvo::Domains::SequenceClassification(DEPTH, ZEROS_UPPER,
                                                      ZEROS_LOWER, DOMAIN_TRACE)
    );

    // Construct experiment
    std::optional<NeuroEvo::Experiment> experiment = NeuroEvo::Experiment::construct(*domain, 
                                                                                     *geno_spec, 
                                                                                     *pheno_spec);

    //Do not continue if experiment construction was not successful
    if(!experiment) exit(0);
    
    //Define genetic operators and parameters
    const unsigned POP_SIZE = 150;
    const unsigned MAX_GENS = 1000;
    const double MUTATION_RATE = 0.4;
    const double MUTATION_POWER = 1.0;

    std::unique_ptr<NeuroEvo::Mutators::Mutation> mutator(
        new NeuroEvo::Mutators::RealGaussianMutation(MUTATION_RATE, MUTATION_POWER)
    );

    std::unique_ptr<NeuroEvo::Selectors::Selection> selector(
        new NeuroEvo::Selectors::RouletteWheelSelection()
    );

    // Run either an evolutionary run or an individual run
    if(argc == 1) experiment->evolutionary_run(POP_SIZE, MAX_GENS, *mutator, *selector);
    if(argc == 2) experiment->individual_run(argv[1]);

}
