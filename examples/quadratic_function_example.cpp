/*
    This is a simple example of how to use NeuroEvo.
    This example maximises a quadratic function.
    The aim here is to illustrate that NeuroEvo can be
    used for mathematical optimisation not just domains
    that use neural networks.
    Futhermore, this example illustrates the used of the CMAES optimiser
    as oppose to the standard genetic algorithm.
*/

#include <experiment.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>
#include <domains/mathematical_functions/quadratic_function.h>
#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutator.h>
#include <gp_map/vector_maps/vector_map.h>
#include <optimiser/cmaes.h>

int main(int argc, const char* argv[]) 
{

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run: ./quadratic_function_example" << std::endl;
        std::cout 
            << "Individual run: ./quadratic_function_example *exp dir*/*population directory*" 
            << std::endl;
        return -1;
    }

    typedef double gene_type;
    typedef double phenotype_output;

    const unsigned num_genes = 1;

    // This is the identity map, the genotype is the same as the phenotype
    std::unique_ptr<NeuroEvo::GPMap<gene_type, phenotype_output>> gp_map(
        new NeuroEvo::VectorMap<gene_type>(new NeuroEvo::VectorPhenotypeSpec(num_genes))
    );

    // Build quadratic function domain
    const double a = -1;
    const double b = 22;
    const double c = -120;

    bool domain_trace = false;

    // if it is an individual run, change domain_trace to true
    if(argc == 2) domain_trace = true;

    std::unique_ptr<NeuroEvo::Domain<gene_type, phenotype_output>> domain(
        new NeuroEvo::QuadraticFunction<gene_type>(a, b, c, domain_trace, 0.9)
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

    //Specify mutator
    const double mutation_rate = 0.4;
    const double mutation_power = 1.0;
    std::shared_ptr<NeuroEvo::Mutator<gene_type>> mutator(
        new NeuroEvo::RealGaussianMutator(mutation_rate, mutation_power)
    );

    // Selector
    std::unique_ptr<NeuroEvo::Selection<gene_type, phenotype_output>> selector(
        new NeuroEvo::RouletteWheelSelection<gene_type, phenotype_output>()
    );

    // CMAES as optimiser
    const std::vector<double> init_mean(num_genes, 0.);
    const double init_sigma = 1.;

    std::unique_ptr<NeuroEvo::Optimiser<gene_type, phenotype_output>> optimiser =
        std::make_unique<NeuroEvo::CMAES<phenotype_output>>(
       init_mean,
       init_sigma,
       *gp_map,
       num_genes,
       max_gens,
       pop_size
    );

    // Run either an evolutionary run or an individual run
    if(argc == 1) experiment->evolutionary_run(*optimiser);
    if(argc == 2) experiment->individual_run(argv[1]);

}
