#include <experiment.h>
#include <sstream>

namespace NeuroEvo {

std::optional<Experiment> 
    Experiment::construct(Domains::Domain& domain,
                          Genotypes::GenotypeSpec& geno_spec,
                          Phenotypes::PhenotypeSpec& pheno_spec,
                          GPMaps::GPMapSpec* gp_map_spec) {

    //Check phenotype specification is appropriate for domain
    if(domain.check_phenotype_spec(pheno_spec))
        return Experiment(domain, geno_spec, pheno_spec, gp_map_spec);
    return std::nullopt;

}

Experiment::Experiment(Domains::Domain& domain,
                       Genotypes::GenotypeSpec& geno_spec,
                       Phenotypes::PhenotypeSpec& pheno_spec,
                       GPMaps::GPMapSpec* gp_map_spec) :
    _domain(domain), 
    _geno_spec(geno_spec),
    _pheno_spec(pheno_spec),
    _gp_map_spec(gp_map_spec) {}

//Runs an individual run with the specified organism folder name
void Experiment::individual_run(const std::string& organism_folder_name) {

    // View the run of the saved best_winner_so_far
    std::stringstream best_winner_path;
    best_winner_path << DATA_PATH << "/" << organism_folder_name << "/best_winner_so_far";

    Organism organism(_geno_spec, _pheno_spec, _gp_map_spec, best_winner_path.str());

    // Run
    // TODO: Move number of trials as function argument
    const unsigned NUM_TRIALS = 1;
    double fitness = _domain.evaluate_org(organism, NUM_TRIALS);

    std::cout << "Individual run fitness: " << fitness << std::endl;

}

void Experiment::evolutionary_run(const unsigned POP_SIZE,
                                  const unsigned MAX_GENS,
                                  Mutators::Mutation& mutator,
                                  Selectors::Selection& selector,
                                  const bool PARALLEL,
                                  const unsigned NUM_RUNS,
                                  const unsigned NUM_TRIALS) {

    for(unsigned i = 0; i < NUM_RUNS; i++) {

        unsigned gen = 1;
        int ga_completed = 0;

        // Build population
        Population population(POP_SIZE, gen, _geno_spec, _pheno_spec, _gp_map_spec);

        // Create a data collector for printing out generational information
        DataCollector data_collector;

        do {

            std::cout << "Gen: " << gen << std::endl;

            // Evaluate population
            _domain.evaluate_population(population, NUM_TRIALS, PARALLEL);

            // Check for completion
            ga_completed = ga_finished(population, MAX_GENS);

            // Print population data after fitness evaluation
            data_collector.collect_generational_data(population);

            // Break if completed
            if(ga_completed != 0) break;

            // Generate new population using genetic operators
            population.generate_new_population(&selector, &mutator, nullptr);

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

//Determines the status of the GA
int Experiment::ga_finished(Population& population, const unsigned MAX_GENS) {

    if(population.get_gen_num() >= MAX_GENS)
        return 2;

    if(_domain.complete())
        return 1;

    return 0;

}

} // namespace NeuroEvo
