/*
    This example shows how an indirect encoding
    (inverse DCT) can be employed to solve the AND
    boolean domain.
*/

#include <experiment.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <domains/boolean_functions/and.h>
#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutator.h>
#include <util/random/uniform_real_distribution.h>
#include <gp_map/dct_map.h>

int main(int argc, const char* argv[]) 
{

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run:   ./inverse_dct_example" << std::endl;
        std::cout << "Individual run:     ./inverse_dct_example *exp_dir*/*population directory*" 
            << std::endl;
        return -1;
    }

    typedef double gene_type;
    typedef double phenotype_output;

    // Specify a network with 2 input nodes, 1 output node,
    // 0 hidden layers and no recurrence.
    const unsigned num_inputs = 2;
    const unsigned num_outputs = 1;
    const unsigned num_hidden_layers = 0;
    const unsigned neurons_per_layer = 0;
    auto pheno_spec = new NeuroEvo::NetworkBuilder(num_inputs, num_outputs,
                                                   num_hidden_layers, neurons_per_layer);

    // Specify the distribution used for the initial gene values
    const double init_gene_lower_bound = 0;
    const double init_gene_upper_bound = 1;
    std::unique_ptr<NeuroEvo::Distribution<gene_type>> genotype_distr(
        new NeuroEvo::UniformRealDistribution(init_gene_lower_bound, init_gene_upper_bound)
    );

    // Specify genotype and mutator
    // This is done after specifying phenotype so the number
    // of genes required is known.
    const double mutation_rate = 0.4;
    const double mutation_power = 1.0;

    std::shared_ptr<NeuroEvo::Mutator<gene_type>> mutator(
        new NeuroEvo::RealGaussianMutator(mutation_rate, mutation_power)
    );

    std::unique_ptr<NeuroEvo::GenotypeSpec<gene_type>> geno_spec(
        new NeuroEvo::GenotypeSpec<gene_type>(pheno_spec->get_num_params(), *genotype_distr,
                                              mutator)
    );

    // Specify an inverse DCT map
    // C is the number of frequency coefficients to be used in
    // the mapping.
    const unsigned c = 3;
    std::unique_ptr<NeuroEvo::GPMap<gene_type, phenotype_output>> gp_map(
        new NeuroEvo::DCTMap(c, num_outputs, num_inputs+1, pheno_spec)
    );

    // Build AND domain
    bool domain_trace = false;

    // If it is an individual run, change domain_trace to true
    if(argc == 2) domain_trace = true;

    std::unique_ptr<NeuroEvo::Domain<gene_type, phenotype_output>> domain(
        new NeuroEvo::AND<gene_type>(domain_trace)
    );

    //Constuct experiment
    std::optional<NeuroEvo::Experiment<gene_type, phenotype_output>> experiment = 
        NeuroEvo::Experiment<gene_type, phenotype_output>::construct(
            *domain, 
            *geno_spec, 
            *gp_map
        );

    //Do not continue if experiment construction was not successful
    if(!experiment) exit(0);

    //Define genetic operators and parameters
    const unsigned pop_size = 150;
    const unsigned max_gens = 1000;

    std::unique_ptr<NeuroEvo::Selection<gene_type, phenotype_output>> selector(
        new NeuroEvo::RouletteWheelSelection<gene_type, phenotype_output>()
    );

    // Run either an evolutionary run or an individual run
    if(argc == 1) experiment->evolutionary_run(pop_size, max_gens, selector.get());
    if(argc == 2) experiment->individual_run(argv[1]);

}
