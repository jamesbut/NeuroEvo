/*
    This example illustrates the single cart pole
    task with a Hebbian network.
*/

#include <population.h>
#include <phenotype/phenotype_specs/hebbs_network_spec.h>
#include <domains/control_domains/single_cart_pole.h>
#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <genetic_operators/mutation/real_gaussian_mutation.h>

//Determines the status of the GA
int ga_finished(Population& population, Domain& domain, const unsigned MAX_GENS) {

    if(population.get_gen_num() >= MAX_GENS)
        return 2;

    if(domain.complete())
        return 1;

    return 0;

}

int main(int argc, const char* argv[]) {

    // Build Hebbs network
    const unsigned NUM_INPUTS = 2;
    const unsigned NUM_OUTPUTS = 2;
    const unsigned NUM_HIDDEN_LAYERS = 0;
    const unsigned NEURONS_PER_LAYER = 0;
    const bool RECURRENT = false;
    const bool RANDOM_WEIGHT_INIT = false;
    const bool EVOLVE_INIT_WEIGHTS = false;
    HebbsNetworkSpec pheno_spec(NUM_INPUTS, NUM_OUTPUTS, NUM_HIDDEN_LAYERS, NEURONS_PER_LAYER,
                                RECURRENT, RANDOM_WEIGHT_INIT, EVOLVE_INIT_WEIGHTS);

    // Build single cart pole domain
    const bool DOMAIN_TRACE = false;
    const bool MARKOVIAN = false;
    const bool RANDOM_START = false;
    const bool PRINT_STATE = false;
    std::unique_ptr<Domain> domain(new SingleCartPole(MARKOVIAN, RANDOM_START,
                                                      PRINT_STATE, DOMAIN_TRACE));

    // Check phenotype is suitable for the specific domain
    if(!domain->check_phenotype_spec(pheno_spec)) exit(EXIT_FAILURE);

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
        Population population(POP_SIZE, gen, pheno_spec);

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
