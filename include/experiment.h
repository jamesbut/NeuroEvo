#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

/*
    An experiment runs either an indivdual run or an evolutionary run according to a
    specification.
*/

#include <population.h>
#include <domains/domain.h>

namespace NeuroEvo {

template <typename G, typename T>
class Experiment {

public:

    //Constructs an experiment conditional on whether the phenotype specification is appropriate
    //for the domain
    static std::optional<Experiment> construct(Domain<G, T>& domain,
                                               GenotypeSpec<G>& geno_spec,
                                               GPMap<G, T>& gp_map,
                                               const bool dump_data = true) 
    {
        
        //Check phenotype specification is appropriate for domain
        if(domain.check_phenotype_spec(*gp_map.get_pheno_spec()))
            return Experiment(domain, geno_spec, gp_map, dump_data);
        return std::nullopt;
        
    }
    
    void individual_run(const std::string& organism_folder_name) 
    {

        // View the run of the saved best_winner_so_far
        std::stringstream best_winner_path;
        best_winner_path << DATA_PATH << "/" << organism_folder_name << "/best_winner_so_far";

        Organism organism(_geno_spec, _gp_map, best_winner_path.str());

        // Run
        // TODO: Move number of trials as function argument
        const unsigned num_trials = 1;
        double fitness = _domain.evaluate_org(organism, num_trials);

        std::cout << "Individual run fitness: " << fitness << std::endl;

    }

    void evolutionary_run(const unsigned pop_size,
                          const unsigned max_gens,
                          Selection<G, T>* selector,
                          const unsigned num_runs = 1,
                          const bool trace = true,
                          const unsigned num_trials = 1,
                          const bool parallel = false)
    {

        if(selector == nullptr)
            std::cout << "NOTE: No selector provided to evolutionary run!" << std::endl;

        unsigned num_winners = 0;
        
        for(unsigned i = 0; i < num_runs; i++) 
        {

            std::cout << "Run: " << i << std::endl;

            unsigned gen = 1;
            int ga_completed = 0;

            // Build population
            Population population(pop_size, gen, _geno_spec, _gp_map);

            do {

                // Evaluate population
                _domain.evaluate_population(population, num_trials, parallel);

                // Check for completion
                ga_completed = ga_finished(population, max_gens);

                // Print population data after fitness evaluation
                _data_collector.collect_generational_data(population, trace);

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
                num_winners++;

            } else if (ga_completed == 2) {

                std::cout << "GA finished at gen: " << gen << " with no winner :(" << std::endl;

            }

            _domain.reset();
            _data_collector.reset();

        }

        std::cout << "Num winners: " << num_winners << "/" << num_runs;

    }

    const DataCollector<G, T>& get_data_collector() const
    {
        return _data_collector;
    }

private:

    //Cannot construct experiment because it is dependent on valid specifications
    Experiment(Domain<G, T>& domain,
               GenotypeSpec<G>& geno_spec,
               GPMap<G, T>& gp_map,
               const bool dump_data) :
        _domain(domain), 
        _geno_spec(geno_spec),
        _gp_map(gp_map),
        _data_collector(dump_data) {}

    int ga_finished(Population<G, T>& population, const unsigned max_gens) 
    {

        if(population.get_gen_num() >= max_gens)
            return 2;

        if(_domain.complete())
            return 1;

        return 0;

    }

    //In most cases the domain and genotype spec will be the same for an 
    //evolutionary run and an individual run so they are saved as member variables and taken as 
    //constructor arguments
    Domain<G, T>& _domain;
    GenotypeSpec<G>& _geno_spec;
    GPMap<G, T>& _gp_map;

    DataCollector<G, T> _data_collector;

};

} // namespace NeuroEvo

#endif
