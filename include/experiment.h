#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

/*
    An experiment runs either an indivdual run or an evolutionary run according to a
    specification.
*/

#include <population.h>
#include <domains/domain.h>

namespace NeuroEvo {

template <typename G>
class Experiment {

public:

    //Constructs an experiment conditional on whether the phenotype specification is appropriate
    //for the domain
    static std::optional<Experiment> construct(Domain<G>& domain,
                                               GenotypeSpec<G>& geno_spec,
                                               PhenotypeSpec<G>& pheno_spec,
                                               GPMapSpec<G>* gp_map_spec = nullptr) 
    {

        //Check phenotype specification is appropriate for domain
        if(domain.check_phenotype_spec(pheno_spec))
            return Experiment(domain, geno_spec, pheno_spec, gp_map_spec);
        return std::nullopt;

    }

    void individual_run(const std::string& organism_folder_name) 
    {

        // View the run of the saved best_winner_so_far
        std::stringstream best_winner_path;
        best_winner_path << DATA_PATH << "/" << organism_folder_name << "/best_winner_so_far";

        Organism organism(_geno_spec, _pheno_spec, _gp_map_spec, best_winner_path.str());

        // Run
        // TODO: Move number of trials as function argument
        const unsigned num_trials = 1;
        double fitness = _domain.evaluate_org(organism, num_trials);

        std::cout << "Individual run fitness: " << fitness << std::endl;

    }

    void evolutionary_run(const unsigned pop_size,
                          const unsigned max_gens,
                          Selection<G>* selector,
                          const bool trace = true,
                          const bool parallel = false,
                          const unsigned num_runs = 1,
                          const unsigned num_trials = 1) 
    {

        if(selector == nullptr)
            std::cout << "NOTE: No selector provided to evolutionary run!" << std::endl;

        for(unsigned i = 0; i < num_runs; i++) {

            unsigned gen = 1;
            int ga_completed = 0;

            // Build population
            Population population(pop_size, gen, _geno_spec, _pheno_spec, _gp_map_spec);

            // Create a data collector for printing out generational information
            DataCollector<G> data_collector;

            do {

                // Evaluate population
                _domain.evaluate_population(population, num_trials, parallel);

                std::cout << population;

                // Check for completion
                ga_completed = ga_finished(population, max_gens);

                // Print population data after fitness evaluation
                data_collector.collect_generational_data(population, trace);

                // Break if completed
                if(ga_completed != 0) break;

                // Generate new population using genetic operators
                population.generate_new_population(selector, nullptr);

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

private:

    //Cannot construct experiment because it is dependent on valid specifications
    Experiment(Domain<G>& domain,
               GenotypeSpec<G>& geno_spec,
               PhenotypeSpec<G>& pheno_spec,
               GPMapSpec<G>* gp_map_spec) :
        _domain(domain), 
        _geno_spec(geno_spec),
        _pheno_spec(pheno_spec),
        _gp_map_spec(gp_map_spec) {}

    int ga_finished(Population<G>& population, const unsigned max_gens) 
    {

        if(population.get_gen_num() >= max_gens)
            return 2;

        if(_domain.complete())
            return 1;

        return 0;

    }

    //In most cases the domain, genotype spec and phenotype spec will be the same for an 
    //evolutionary run and an individual run so they are saved as member variables and taken as 
    //constructor arguments
    Domain<G>& _domain;
    GenotypeSpec<G>& _geno_spec;
    PhenotypeSpec<G>& _pheno_spec;
    GPMapSpec<G>* _gp_map_spec;

};

} // namespace NeuroEvo

#endif
