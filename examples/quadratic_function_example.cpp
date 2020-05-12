/*
    This is a simple example of how to use NeuroEvo.
    This example maximises a quadratic function.
    The aim here is to illustrate that NeuroEvo can be
    used for mathematical optimisation not just domains
    that use neural networks
*/

#include <experiment.h>
#include <phenotype/phenotype_specs/real_vector_phenotype_spec.h>
#include <domains/mathematical_functions/quadratic_function.h>
#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutation.h>
#include <util/random/uniform_distribution.h>

int main(int argc, const char* argv[]) {

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run: ./quadratic_function_example" << std::endl;
        std::cout << "Individual run:   ./quadratic_function_example *population directory*" << std::endl;
        return -1;
    }

    // Build real vector phenotype - no network needed for
    // mathematical optimisation
    const unsigned NUM_GENES = 1;
    std::unique_ptr<NeuroEvo::Phenotypes::PhenotypeSpec> pheno_spec(
        new NeuroEvo::Phenotypes::RealVectorPhenotypeSpec(NUM_GENES)
    );

    // Specify the distribution used for the initial gene values
    const double INIT_GENE_LOWER_BOUND = 0;
    const double INIT_GENE_UPPER_BOUND = 1;
    std::unique_ptr<NeuroEvo::Utils::Distribution> genotype_distr(
        new NeuroEvo::Utils::UniformDistribution(INIT_GENE_LOWER_BOUND, INIT_GENE_UPPER_BOUND)
    );

    // Specify genotype
    std::unique_ptr<NeuroEvo::Genotypes::GenotypeSpec> geno_spec(
        new NeuroEvo::Genotypes::GenotypeSpec(NUM_GENES, *genotype_distr)
    );

    // Build quadratic function domain
    const double A = -1;
    const double B = 22;
    const double C = -120;

    bool DOMAIN_TRACE = false;

    // If it is an individual run, change domain_trace to true
    if(argc == 2) DOMAIN_TRACE = true;

    std::unique_ptr<NeuroEvo::Domains::Domain> domain(
        new NeuroEvo::Domains::QuadraticFunction(A, B, C, DOMAIN_TRACE, 0.9)
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
