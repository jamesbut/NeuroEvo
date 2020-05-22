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
#include <genetic_operators/selection/truncation_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutator.h>
#include <util/random/uniform_real_distribution.h>

int main(int argc, const char* argv[]) 
{

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run: ./quadratic_function_example" << std::endl;
        std::cout << "Individual run:   ./quadratic_function_example *population directory*" 
            << std::endl;
        return -1;
    }

    typedef double gene_type;

    // Build real vector phenotype - no network needed for
    // mathematical optimisation
    const unsigned num_genes = 1;
    std::unique_ptr<NeuroEvo::PhenotypeSpec<gene_type>> pheno_spec(
        new NeuroEvo::RealVectorPhenotypeSpec(num_genes)
    );

    // Specify the distribution used for the initial gene values
    const double init_gene_lower_bound = 0;
    const double init_gene_upper_bound = 1;
    std::unique_ptr<NeuroEvo::Distribution<gene_type>> genotype_distr(
        new NeuroEvo::UniformRealDistribution(init_gene_lower_bound, init_gene_upper_bound)
    );

    // Specify genotype
    std::unique_ptr<NeuroEvo::GenotypeSpec<gene_type>> geno_spec(
        new NeuroEvo::GenotypeSpec<gene_type>(num_genes, *genotype_distr)
    );

    // Build quadratic function domain
    const double a = -1;
    const double b = 22;
    const double c = -120;

    bool domain_trace = false;

    // if it is an individual run, change domain_trace to true
    if(argc == 2) domain_trace = true;

    std::unique_ptr<NeuroEvo::Domain<gene_type>> domain(
        new NeuroEvo::QuadraticFunction<gene_type>(a, b, c, domain_trace, 0.9)
    );

    // Construct experiment
    std::optional<NeuroEvo::Experiment<gene_type>> experiment = 
        NeuroEvo::Experiment<gene_type>::construct(*domain, 
                                                   *geno_spec, 
                                                   *pheno_spec);

    //Do not continue if experiment construction was not successful
    if(!experiment) exit(0);
    
    //Define genetic operators and parameters
    //const unsigned pop_size = 150;
    const unsigned pop_size = 10;
    //const unsigned max_gens = 1000;
    const unsigned max_gens = 2;
    const double mutation_rate = 0.4;
    const double mutation_power = 1.0;

    std::unique_ptr<NeuroEvo::Mutator<gene_type>> mutator(
        new NeuroEvo::RealGaussianMutator(mutation_rate, mutation_power)
    );

    std::unique_ptr<NeuroEvo::Selection<gene_type>> selector(
        //new NeuroEvo::RouletteWheelSelection<gene_type>()
        new NeuroEvo::TruncationSelection<gene_type>(0.4)
    );

    // Run either an evolutionary run or an individual run
    if(argc == 1) experiment->evolutionary_run(pop_size, max_gens, nullptr);
    if(argc == 2) experiment->individual_run(argv[1]);

}
