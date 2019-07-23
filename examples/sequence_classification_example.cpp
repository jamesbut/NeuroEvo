/*
    This example shows how a network solves the
    sequence classification task - a good task for
    testing a networks memory capability.
    This example also illustrates how you can alternatively
    build a network specification - layer by layer.
    It also illustrates the use fo GRUs in a network.
*/

#include <population.h>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <phenotype/phenotype_specs/layer_spec.h>
#include <domains/sequence_classification.h>
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
    ActivationFuncSpec* hl_activation_spec(new SigmoidSpec(1));
    LayerSpec hidden_layer(HL_LAYER_TYPE, HL_NUM_NEURONS, HL_INPUTS_PER_NEURON, hl_activation_spec);

    const unsigned OL_LAYER_TYPE = 0;
    const unsigned OL_NUM_NEURONS = 1;
    const unsigned OL_INPUTS_PER_NEURON = 3;    // The number of inputs to the output layer
    ActivationFuncSpec* ol_activation_spec(new SigmoidSpec(1));
    LayerSpec output_layer(OL_LAYER_TYPE, OL_NUM_NEURONS, OL_INPUTS_PER_NEURON, ol_activation_spec);

    // Build a network from the layer specifications
    std::vector<LayerSpec> layer_specs{hidden_layer, output_layer};

    const unsigned NUM_INPUTS = 1;
    FixedNetworkSpec pheno_spec(NUM_INPUTS, layer_specs);

    // Build sequence classification domain
    const bool DOMAIN_TRACE = false;
    const unsigned DEPTH = 3;           // The number of 1s and -1s
    const unsigned ZEROS_LOWER = 5;     // The smallest number of zeros interleaved in
    const unsigned ZEROS_UPPER = 5;     // The highest number of zeros interleaved in
    std::unique_ptr<Domain> domain(new SequenceClassification(DEPTH, ZEROS_UPPER, ZEROS_LOWER,
                                                              DOMAIN_TRACE));

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
