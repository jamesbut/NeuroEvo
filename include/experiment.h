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
#include <optimiser/optimiser.h>

namespace NeuroEvo {

template <typename G, typename T>
class Experiment {

public:

    //Constructs an experiment conditional on whether the phenotype specification is 
    //appropriate for the domain
    static std::optional<Experiment> construct(Domain<G, T>& domain,
                                               GPMap<G, T>& gp_map,
                                               const bool dump_data = true,
                                               const bool dump_winners_only = false) 
    {
        
        //Check phenotype specification is appropriate for domain
        if(domain.check_phenotype_spec(*gp_map.get_pheno_spec()))
            return Experiment(domain, gp_map, dump_data, dump_winners_only);
        return std::nullopt;
        
    }
    
    void individual_run(const std::string& organism_folder_name, 
                        const unsigned num_trials = 1,
                        const bool pheno_trace = false,
                        const bool domain_trace = false,
                        const bool render = false) 
    {

        // View the run of the saved best_winner_so_far
        std::stringstream best_winner_path;
        best_winner_path << DATA_PATH << "/" << organism_folder_name << "/best_winner_so_far";

        if(pheno_trace)
            _gp_map.set_pheno_spec_trace(pheno_trace);

        Organism organism(_gp_map, best_winner_path.str());

        _domain.set_render(render);
        _domain.set_trace(domain_trace);

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

    void evolutionary_run(Optimiser<G, T>& optimiser,
                          const unsigned num_runs = 1,
                          const bool parallel_runs = false,
                          const bool trace = true,
                          const bool domain_parallel = false)
    {

        std::clock_t start = std::clock();

        //Create experiment directory
        if(_dump_data)
            _exp_dir_path = std::make_optional(DataCollector<G, T>::create_exp_dir());

        //Run runs in parallel
        if(parallel_runs)
        {
            //Trace is off in parallel runs
            const bool trace = false;
            const RunArguments<G, T> run_args{&_domain, optimiser, &_gp_map, _exp_dir_path, 
                                              _dump_winners_only, _num_winners, 
                                              _total_winners_gens, trace, domain_parallel};
            RunScheduler<G, T> scheduler(run_args, num_runs);
            scheduler.dispatch(run);

        } else 
        {
            //This flag is only needed for parallelisation
            bool completed_flag = false;
            for(unsigned i = 0; i < num_runs; i++) 
            {
                std::cout << "Starting run: " << i << std::endl;
                run(&_domain, optimiser, &_gp_map, _exp_dir_path, _dump_winners_only, 
                    _num_winners, completed_flag, _total_winners_gens, trace, domain_parallel);
            }
        }

        const double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
        _avg_winners_gens = (double)_total_winners_gens / (double)num_runs;

        std::cout << "Num winners: " << _num_winners << "/" << num_runs << std::endl;
        std::cout << "Average winner generation: " << _avg_winners_gens << std::endl;
        std::cout << "Duration: " << duration << " seconds" << std::endl;

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

    void set_domain_seed(const unsigned seed)
    {
        _domain.set_seed(seed);
    }

    void dump_num_winners(const std::string& file_name)
    {
        std::ofstream file; 
        file.open(file_name, std::ios::app);
        file << _num_winners << std::endl;
        file.close();
    }

    void dump_avg_winners_gens(const std::string& file_name)
    {
        std::ofstream file; 
        file.open(file_name, std::ios::app);
        file << _avg_winners_gens << std::endl;
        file.close();
    }

private:

    //Cannot construct experiment because it is dependent on valid specifications
    Experiment(Domain<G, T>& domain,
               GPMap<G, T>& gp_map,
               const bool dump_data,
               const bool dump_winners_only) :
        _domain(domain), 
        _gp_map(gp_map),
        _exp_dir_path(std::nullopt),
        _dump_data(dump_data),
        _dump_winners_only(dump_winners_only),
        _num_winners(0),
        _total_winners_gens(0),
        _avg_winners_gens(0) {}

    static void run(Domain<G, T>* m_domain,
                    Optimiser<G, T>& a_optimiser,
                    GPMap<G, T>* m_gp_map,
                    const std::optional<const std::string> exp_dir_path,
                    const bool dump_winners_only,
                    unsigned& num_winners,
                    bool& completed_flag,
                    unsigned& total_winners_gens,
                    const bool trace = true,
                    const bool domain_parallel = false)
    {

        //Copy and reset domain
        std::unique_ptr<Domain<G, T>> domain = m_domain->clone();        
        domain->exp_run_reset();

        //Copy and reset optimiser
        std::unique_ptr<Optimiser<G, T>> optimiser = a_optimiser.clone();
        optimiser->reset();

        //Create new data collector
        DataCollector<G, T> data_collector(exp_dir_path, optimiser->get_max_gens(), 
                                           dump_winners_only, trace);

        //Call optimiser
        const bool optimiser_status = optimiser->optimise(*domain, data_collector);
        
        // Check whether the domain was solved or not
        if(optimiser_status) 
        {
            std::cout << "FOUND WINNER!" << std::endl;
            std::cout << "Gen: " << optimiser->get_finished_gen() << std::endl;
            num_winners++;
        } else
            std::cout << "GA finished at gen: " << optimiser->get_finished_gen() 
                << " with no winner :(" << std::endl;

        total_winners_gens += optimiser->get_finished_gen();
        completed_flag = true;

    }

    //In most cases the domain and genotype spec will be the same for an 
    //evolutionary run and an individual run so they are saved as member variables 
    //and taken as constructor arguments
    Domain<G, T>& _domain;
    GPMap<G, T>& _gp_map;

    std::optional<std::string> _exp_dir_path;
    const bool _dump_data;
    const bool _dump_winners_only;

    unsigned _num_winners;
    unsigned _total_winners_gens;
    unsigned _avg_winners_gens;

};

} // namespace NeuroEvo

#endif
