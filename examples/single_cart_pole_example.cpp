/*
    This example illustrates the single cart pole
    task with a Hebbian network.
*/

#include <experiment.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <domains/control_domains/single_cart_pole.h>
#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutator.h>
#include <util/random/uniform_real_distribution.h>
#include <gp_map/vector_to_network_map.h>

int main(int argc, const char* argv[]) 
{

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run:   ./single_cart_pole_example" << std::endl;
        std::cout << 
            "Individual run:     ./single_cart_pole_example *exp dir*/*population directory*" 
            << std::endl;
        return -1;
    }

    typedef double gene_type;
    typedef double phenotype_output;

    // Build Hebbs network
    const unsigned num_inputs = 2;
    const unsigned num_outputs = 2;
    const unsigned num_hidden_layers = 0;
    const unsigned neurons_per_layer = 0;
    auto network_builder = new NeuroEvo::NetworkBuilder(num_inputs, num_outputs,
                                                        num_hidden_layers, neurons_per_layer);

    const bool evolve_init_weights = false;
    const double default_init_weight = 1.;
    network_builder->make_hebbian(evolve_init_weights, default_init_weight);

    // Specify the distribution used for the initial gene values
    const double init_gene_lower_bound = 0;
    const double init_gene_upper_bound = 1;
    std::unique_ptr<NeuroEvo::Distribution<gene_type>> genotype_distr(
        new NeuroEvo::UniformRealDistribution(init_gene_lower_bound, init_gene_upper_bound)
    );

    // Specify genotype and mutator
    const double mutation_rate = 0.4;
    const double mutation_power = 1.0;
    std::shared_ptr<NeuroEvo::Mutator<gene_type>> mutator(
        new NeuroEvo::RealGaussianMutator(mutation_rate, mutation_power)
    );

    std::unique_ptr<NeuroEvo::GenotypeSpec<gene_type>> geno_spec(
        new NeuroEvo::GenotypeSpec<gene_type>(network_builder->get_num_params(), *genotype_distr,
                                              mutator)
    );

    std::unique_ptr<NeuroEvo::GPMap<gene_type, phenotype_output>> gp_map(
        new NeuroEvo::VectorToNetworkMap(network_builder)
    );

    // Build single cart pole domain
    const bool markovian = false;
    const bool random_start = false;
    const bool print_state = false;

    bool domain_trace = false;
    //One can turn rendering on here
    bool render = false;
    if(argc == 2) 
    {
        domain_trace = false;
        render = true;
    }

    std::unique_ptr<NeuroEvo::Domain<gene_type, phenotype_output>> domain(
        new NeuroEvo::SingleCartPole<gene_type>(markovian, random_start,
                                                print_state, domain_trace,
                                                render)
    );

    // Construct experiment
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
