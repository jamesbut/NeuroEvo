/*
    This is a simple example of how to use NeuroEvo.
    This example maximises a quadratic function.
    The aim here is to illustrate that NeuroEvo can be
    used for mathematical optimisation not just domains
    that use neural networks
*/

#include <population.h>
#include <phenotype/phenotype_specs/real_vector_phenotype_spec.h>
#include <domains/mathematical_functions/quadratic_function.h>
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

    const unsigned NUM_GENES = 1;
    RealVectorPhenotypeSpec pheno_spec(NUM_GENES);

    // Build quadratic function domain
    const double A = -1;
    const double B = 22;
    const double C = -120;
    const bool DOMAIN_TRACE = false;
    std::unique_ptr<Domain> domain(new QuadraticFunction(A, B, C, DOMAIN_TRACE, 0.9));

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
