#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

/*
    An experiment runs either an indivdual run or an evolutionary run according to a
    specification.
*/

#include <population.h>
#include <domains/domain.h>
#include <util/file_utils.h>
#include <thread>
#include <util/concurrency/run_scheduler.h>

namespace NeuroEvo {

template <typename G, typename T>
class Experiment {

public:

    //Constructs an experiment conditional on whether the phenotype specification is appropriate
    //for the domain
    static std::optional<Experiment> construct(Domain<G, T>& domain,
                                               GenotypeSpec<G>& geno_spec,
                                               GPMap<G, T>& gp_map,
                                               const bool dump_data = true,
                                               const bool dump_winners_only = false) 
    {
        
        //Check phenotype specification is appropriate for domain
        if(domain.check_phenotype_spec(*gp_map.get_pheno_spec()))
            return Experiment(domain, geno_spec, gp_map, dump_data, dump_winners_only);
        return std::nullopt;
        
    }
    
    void individual_run(const std::string& organism_folder_name, 
                        const unsigned num_trials = 1,
                        const bool pheno_trace = false) 
    {

        // View the run of the saved best_winner_so_far
        std::stringstream best_winner_path;
        best_winner_path << DATA_PATH << "/" << organism_folder_name << "/best_winner_so_far";

        if(pheno_trace)
            _gp_map.set_pheno_spec_trace(pheno_trace);

        Organism organism(_geno_spec, _gp_map, best_winner_path.str());

        _domain.set_render(true);
        _domain.set_trace(true);

        double fitness = _domain.evaluate_org(organism, num_trials);

        std::cout << "Individual run fitness: " << fitness << std::endl;

    }

    //Tests and individual given a set of genes
    void individual_run(const std::vector<G>& genes,
                        const unsigned num_trials)
    {
        //_gp_map.set_pheno_spec_trace(true);
        Genotype<G> genotype(genes);
        Organism<G, T> organism(genotype, _gp_map);

        //_domain.set_trace(true);
        double fitness = _domain.evaluate_org(organism, num_trials);

        std::cout << "Individual run fitness: " << fitness << std::endl;

    }

    void evolutionary_run(const unsigned pop_size,
                          const unsigned max_gens,
                          Selection<G, T>* selector,
                          const unsigned num_runs = 1,
                          const bool parallel_runs = false,
                          const bool trace = true,
                          const unsigned num_trials = 1,
                          const bool domain_parallel = false)
    {

        //Create experiment directory
        if(_dump_data)
            _exp_dir_path = std::make_optional(DataCollector<G, T>::create_exp_dir());

        if(selector == nullptr)
            std::cout << "NOTE: No selector provided to evolutionary run!" << std::endl;

        unsigned num_winners = 0;

        //Run runs in parallel
        if(parallel_runs)
        {
            //Trace is off in parallel runs
            const bool trace = false;
            const RunArguments<G, T> run_args{pop_size, max_gens, selector, &_domain, &_geno_spec, 
                                              &_gp_map, _exp_dir_path, _dump_winners_only, 
                                              num_winners, trace, num_trials, domain_parallel};
            RunScheduler<G, T> scheduler(run_args, num_runs);
            scheduler.dispatch(run);

        } else 
        {
            //This flag is only needed for parallelisation
            bool completed_flag = false;
            for(unsigned i = 0; i < num_runs; i++) 
            {
                std::cout << "Starting run: " << i << std::endl;
                run(pop_size, max_gens, selector, &_domain, &_geno_spec, &_gp_map, _exp_dir_path,
                    _dump_winners_only, num_winners, completed_flag, trace, num_trials, 
                    domain_parallel);
            }
        }

        std::cout << "Num winners: " << num_winners << "/" << num_runs << std::endl;

    }

    const std::optional<std::string>& get_exp_dir_path() const
    {
        return _exp_dir_path;
    }

    std::vector<std::string> get_run_dir_paths() const
    {
        if(_exp_dir_path.has_value())
            return collect_dirs_in(_exp_dir_path.value());
        else
        {
            std::cerr << "_exp_dir_path does not have a value in order to collect run dirs" <<
                std::endl;
            exit(0);
        }
    }

    void set_domain_trace(const bool domain_trace)
    {
        _domain.set_trace(domain_trace);
    }


private:

    //Cannot construct experiment because it is dependent on valid specifications
    Experiment(Domain<G, T>& domain,
               GenotypeSpec<G>& geno_spec,
               GPMap<G, T>& gp_map,
               const bool dump_data,
               const bool dump_winners_only) :
        _domain(domain), 
        _geno_spec(geno_spec),
        _gp_map(gp_map),
        _exp_dir_path(std::nullopt),
        _dump_data(dump_data),
        _dump_winners_only(dump_winners_only) {}

    static int ga_finished(Population<G, T>& population, Domain<G, T>& domain, 
                           const unsigned max_gens) 
    {

        if(population.get_gen_num() >= max_gens)
            return 2;

        if(domain.complete())
            return 1;

        return 0;

    }

    static void run(const unsigned pop_size,
                    const unsigned max_gens,
                    Selection<G, T>* selector,
                    Domain<G, T>* m_domain,
                    GenotypeSpec<G>* m_geno_spec,
                    GPMap<G, T>* m_gp_map,
                    const std::optional<const std::string>& exp_dir_path,
                    const bool dump_winners_only,
                    unsigned& num_winners,
                    bool& completed_flag,
                    const bool trace = true,
                    const unsigned num_trials = 1,
                    const bool domain_parallel = false)
    {

        //Copy domain
        std::unique_ptr<Domain<G, T>> domain = m_domain->clone();        

        //Create new data collector
        DataCollector<G, T> data_collector(exp_dir_path, max_gens, dump_winners_only);

        unsigned gen = 1;
        int ga_completed = 0;

        // Build population
        Population population(pop_size, gen, *m_geno_spec, *m_gp_map);

        do {

            // Evaluate population
            domain->evaluate_population(population, num_trials, domain_parallel);

            // Check for completion
            ga_completed = ga_finished(population, *domain, max_gens);
            bool final_gen = ga_completed == 0 ? false : true;

            // Print population data after fitness evaluation
            data_collector.collect_generational_data(population, final_gen, trace);

            // Break if completed
            if(final_gen) break;

            // Generate new population using genetic operators
            population.generate_new_population(selector, nullptr);

            gen++;

        } while (ga_completed == 0);
        
        // Check whether the domain was solved or not
        if(ga_completed == 1) {

            //if(trace)
            std::cout << "FOUND WINNER!" << std::endl;
            std::cout << "Gen: " << gen << std::endl;
            num_winners++;

        } else if (ga_completed == 2) {

            std::cout << "GA finished at gen: " << gen << " with no winner :(" << std::endl;

        }

        completed_flag = true;

    }

    //In most cases the domain and genotype spec will be the same for an 
    //evolutionary run and an individual run so they are saved as member variables and taken as 
    //constructor arguments
    Domain<G, T>& _domain;
    GenotypeSpec<G>& _geno_spec;
    GPMap<G, T>& _gp_map;

    std::optional<std::string> _exp_dir_path;
    const bool _dump_data;
    const bool _dump_winners_only;

};

} // namespace NeuroEvo

#endif
