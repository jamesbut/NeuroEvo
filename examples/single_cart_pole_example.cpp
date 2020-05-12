/*
    This example illustrates the single cart pole
    task with a Hebbian network.
*/

#include <experiment.h>
#include <phenotype/phenotype_specs/hebbs_network_spec.h>
#include <domains/control_domains/single_cart_pole.h>
#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutation.h>
#include <util/random/uniform_distribution.h>

int main(int argc, const char* argv[]) {

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run:   ./single_cart_pole_example" << std::endl;
        std::cout << "Individual run:     ./single_cart_pole_example *population directory*" << std::endl;
        return -1;
    }

    // Build Hebbs network
    const unsigned NUM_INPUTS = 2;
    const unsigned NUM_OUTPUTS = 2;
    const unsigned NUM_HIDDEN_LAYERS = 0;
    const unsigned NEURONS_PER_LAYER = 0;
    const bool RECURRENT = false;
    const bool RANDOM_WEIGHT_INIT = false;
    const bool EVOLVE_INIT_WEIGHTS = false;
    std::unique_ptr<NeuroEvo::Phenotypes::PhenotypeSpec> pheno_spec(
        new NeuroEvo::Phenotypes::HebbsNetworkSpec(NUM_INPUTS, NUM_OUTPUTS,
                                                   NUM_HIDDEN_LAYERS, NEURONS_PER_LAYER,
                                                   RECURRENT, RANDOM_WEIGHT_INIT,
                                                   EVOLVE_INIT_WEIGHTS)
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

    // Build single cart pole domain
    const bool MARKOVIAN = false;
    const bool RANDOM_START = false;
    const bool PRINT_STATE = false;

    bool DOMAIN_TRACE = false;
    if(argc == 2) DOMAIN_TRACE = true;

    std::unique_ptr<NeuroEvo::Domains::Domain> domain(
        new NeuroEvo::Domains::SingleCartPole(MARKOVIAN, RANDOM_START,
                                              PRINT_STATE, DOMAIN_TRACE)
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
