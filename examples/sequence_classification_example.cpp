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
#include <sstream>

//Determines the status of the GA
int ga_finished(NeuroEvo::Population& population, NeuroEvo::Domains::Domain& domain,
                const unsigned MAX_GENS) {

    if(population.get_gen_num() >= MAX_GENS)
        return 2;

    if(domain.complete())
        return 1;

    return 0;

}

void individual_run(std::unique_ptr<NeuroEvo::Domains::Domain>& domain,
                    std::unique_ptr<NeuroEvo::Phenotypes::PhenotypeSpec>& pheno_spec,
                    const std::string& organism_folder_name) {

    // View the run of the saved best_winner_so_far
    std::stringstream best_winner_path;
    best_winner_path << DATA_PATH << "/" << organism_folder_name << "/best_winner_so_far";

    NeuroEvo::Organism organism(*pheno_spec, best_winner_path.str());

    // Run
    const unsigned NUM_TRIALS = 1;
    double fitness = domain->evaluate_org(organism, NUM_TRIALS);

    std::cout << "Individual run fitness: " << fitness << std::endl;

}

void evolutionary_run(std::unique_ptr<NeuroEvo::Domains::Domain>& domain,
                      std::unique_ptr<NeuroEvo::Phenotypes::PhenotypeSpec>& pheno_spec) {

    // Build genetic operators
    const double MUTATION_RATE = 0.4;
    const double MUTATION_POWER = 1.0;
    std::unique_ptr<NeuroEvo::Mutators::Mutation> mutator(
        new NeuroEvo::Mutators::RealGaussianMutation(MUTATION_RATE, MUTATION_POWER)
    );

    std::unique_ptr<NeuroEvo::Selectors::Selection> selector(
        new NeuroEvo::Selectors::RouletteWheelSelection()
    );

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
        NeuroEvo::Population population(POP_SIZE, gen, *pheno_spec);

        // Create a data collector for printing out generational information
        NeuroEvo::DataCollector data_collector;

        do {

            std::cout << "Gen: " << gen << std::endl;

            // Evaluate population
            domain->evaluate_population(population, NUM_TRIALS, PARALLEL);

            // Check for completion
            ga_completed = ga_finished(population, *domain, MAX_GENS);

            // Print population data after fitness evaluation
            data_collector.collect_generational_data(population);

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
        std::cout << "Evolutionary run: ./sequence_classification_example" << std::endl;
        std::cout << "Individual run:   ./sequence_classification_example *population directory*"
                  << std::endl;

        return -1;
    }

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
    NeuroEvo::Phenotypes::ActivationFuncSpec* hl_activation_spec(new NeuroEvo::Phenotypes::SigmoidSpec(1));
    NeuroEvo::Phenotypes::LayerSpec hidden_layer(HL_LAYER_TYPE, HL_NUM_NEURONS,
                                                 HL_INPUTS_PER_NEURON, hl_activation_spec);

    const unsigned OL_LAYER_TYPE = 0;
    const unsigned OL_NUM_NEURONS = 1;
    const unsigned OL_INPUTS_PER_NEURON = 3;    // The number of inputs to the output layer
    NeuroEvo::Phenotypes::ActivationFuncSpec* ol_activation_spec(new NeuroEvo::Phenotypes::SigmoidSpec(1));
    NeuroEvo::Phenotypes::LayerSpec output_layer(OL_LAYER_TYPE, OL_NUM_NEURONS,
                                                 OL_INPUTS_PER_NEURON, ol_activation_spec);

    // Build a network from the layer specifications
    std::vector<NeuroEvo::Phenotypes::LayerSpec> layer_specs{hidden_layer, output_layer};

    const unsigned NUM_INPUTS = 1;
    std::unique_ptr<NeuroEvo::Phenotypes::PhenotypeSpec> pheno_spec(
        new NeuroEvo::Phenotypes::FixedNetworkSpec(NUM_INPUTS, layer_specs)
    );

    // Build sequence classification domain
    const unsigned DEPTH = 3;           // The number of 1s and -1s
    const unsigned ZEROS_LOWER = 5;     // The smallest number of zeros interleaved in
    const unsigned ZEROS_UPPER = 5;     // The highest number of zeros interleaved in

    bool DOMAIN_TRACE = false;
    if(argc == 2) DOMAIN_TRACE = true;

    std::unique_ptr<NeuroEvo::Domains::Domain> domain(
        new NeuroEvo::Domains::SequenceClassification(DEPTH, ZEROS_UPPER,
                                                      ZEROS_LOWER, DOMAIN_TRACE)
    );

    // Check phenotype is suitable for the specific domain
    if(!domain->check_phenotype_spec(*pheno_spec)) return -1;

    // Run either an evolutionary run or an individual run
    if(argc == 1) evolutionary_run(domain, pheno_spec);
    if(argc == 2) individual_run(domain, pheno_spec, argv[1]);

}
