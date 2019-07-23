/*
    This is a simple example of how to use NeuroEvo.
    This example solves the AND boolean domain with
    a fixed neural network.
*/

#include <population.h>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <domains/boolean_functions/and.h>
#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutation.h>
#include <sstream>

//Determines the status of the GA
int ga_finished(Population& population, Domain& domain, const unsigned MAX_GENS) {

    if(population.get_gen_num() >= MAX_GENS)
        return 2;

    if(domain.complete())
        return 1;

    return 0;

}

void individual_run(std::unique_ptr<Domain>& domain, std::unique_ptr<PhenotypeSpec>& pheno_spec,
                    const std::string& organism_folder_name) {

    // View the run of the saved best_winner_so_far
    std::stringstream best_winner_path;
    best_winner_path << "../../data/" << organism_folder_name << "/best_winner_so_far";

    Organism organism(*pheno_spec, best_winner_path.str());

    // Run
    const unsigned NUM_TRIALS = 1;
    double fitness = domain->evaluate_org(organism, NUM_TRIALS);

    std::cout << "Individual run fitness: " << fitness << std::endl;

}

void evolutionary_run(std::unique_ptr<Domain>& domain, std::unique_ptr<PhenotypeSpec>& pheno_spec) {

    // Build genetic operators
    const double MUTATION_RATE = 0.4;
    const double MUTATION_POWER = 1.0;
    std::unique_ptr<Mutation> mutator(new RealGaussianMutation(MUTATION_RATE, MUTATION_POWER));

    std::unique_ptr<Selection> selector(new RouletteWheelSelection());

    // Evolutionary parameters
    const unsigned NUM_RUNS = 1;
    const unsigned POP_SIZE = 150;
    const unsigned MAX_GENS = 1000;
    const unsigned NUM_TRIALS = 1;
    const bool PARALLEL = false;

    for(unsigned i = 0; i < NUM_RUNS; i++) {

        unsigned gen = 1;
        int ga_completed = 0;

        // Build population
        Population population(POP_SIZE, gen, *pheno_spec);

        do {

            std::cout << "Gen: " << gen << std::endl;

            // Evaluate population
            domain->evaluate_population(population, NUM_TRIALS, PARALLEL);

            // Check for completion
            ga_completed = ga_finished(population, *domain, MAX_GENS);

            // Break if completed
            if(ga_completed != 0) break;

            // Generate new population using genetic operators
            population.generate_new_population(selector.get(), mutator.get(), nullptr);

            gen++;

        } while (ga_completed == 0);

        // Check whether the domain was solved or not
        if(ga_completed == 1) {

            std::cout << "FOUND WINNER!" << std::endl;
            std::cout << "Gen: " << gen << std::endl;

        } else if (ga_completed == 2) {

            std::cout << "GA finished at gen: " << gen << " with no winner :(" << std::endl;

        }

    }

}

int main(int argc, const char* argv[]) {

    //Check for correct command line arguments
    if(argc < 1 || argc > 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "Evolutionary run:   ./and_example" << std::endl;
        std::cout << "Individual run:     ./and_example *population directory*" << std::endl;
        return -1;
    }

    // Build a network with 2 input nodes, 1 output node,
    // 0 hidden layers and no recurrence.
    const unsigned NUM_INPUTS = 2;
    const unsigned NUM_OUTPUTS = 1;
    const unsigned NUM_HIDDEN_LAYERS = 0;
    const unsigned NEURONS_PER_LAYER = 0;
    const bool RECURRENT = false;
    std::unique_ptr<PhenotypeSpec> pheno_spec(new FixedNetworkSpec(NUM_INPUTS, NUM_OUTPUTS,
                                                                   NUM_HIDDEN_LAYERS, NEURONS_PER_LAYER,
                                                                   RECURRENT));

    // Build AND domain
    bool DOMAIN_TRACE = false;

    // If it is an individual run, change domain_trace to true
    if(argc == 2) DOMAIN_TRACE = true;

    std::unique_ptr<Domain> domain(new AND(DOMAIN_TRACE));

    // Check phenotype is suitable for the specific domain
    if(!domain->check_phenotype_spec(*pheno_spec)) return -1;

    // Run either an evolutionary run or an individual run
    if(argc == 1) evolutionary_run(domain, pheno_spec);
    if(argc == 2) individual_run(domain, pheno_spec, argv[1]);

}
