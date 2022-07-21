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
    Experiment(std::shared_ptr<Domain<G, T>> domain,
               std::shared_ptr<GPMap<G, T>> gp_map,
               const bool dump_data,
               const bool dump_winners_only,
               const std::optional<const JSON> exp_json = std::nullopt) :
        _domain(domain),
        _gp_map(gp_map),
        _exp_dir_path(std::nullopt),
        _exp_json(exp_json),
        _dump_data(dump_data),
        _dump_winners_only(dump_winners_only),
        _num_winners(0),
        _total_winners_gens(0),
        _avg_winners_gens(0)
    {
        //Check phenotype specification is appropriate for domain
        if(!domain->check_phenotype_spec(*gp_map->get_pheno_spec()))
            throw std::invalid_argument("Phenotype spec is not appropriate for this "
                                        "domain");
    }

    void evolutionary_run(std::shared_ptr<Optimiser<G, T>> optimiser,
                          const unsigned num_runs = 1,
                          const bool parallel_runs = false,
                          const bool trace = true,
                          const bool domain_parallel = false)
    {

        std::clock_t start = std::clock();

        if(_dump_data)
        {
            //Create experiment directory
            _exp_dir_path = std::make_optional(
                DataCollector<G, T>::create_exp_dir(_exp_json)
            );

            //Dump JSON if there is one
            if(_exp_json.has_value()) 
                _exp_json->save_to_file(_exp_dir_path.value() + "/exp_config");
        }

        //Run runs in parallel
        if(parallel_runs)
        {
            //Trace is off in parallel runs
            const bool trace = false;
            const RunArguments<G, T> run_args{_domain, optimiser, _gp_map,
                                              _exp_dir_path, _dump_winners_only,
                                              _num_winners, _total_winners_gens, trace,
                                              domain_parallel};
            RunScheduler<G, T> scheduler(run_args, num_runs);
            scheduler.dispatch(run);

        } else
        {
            //This flag is only needed for parallelisation
            bool completed_flag = false;
            for(unsigned i = 0; i < num_runs; i++)
            {
                std::cout << "Starting run: " << i << std::endl;
                run(_domain, optimiser, _gp_map, i, _exp_dir_path, _dump_winners_only,
                    _num_winners, completed_flag, _total_winners_gens, trace,
                    domain_parallel);
            }
        }

        //Calculate a few statistics
        const double duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
        _avg_winners_gens = (double)_total_winners_gens / (double)num_runs;

        const unsigned non_winners = num_runs - _num_winners;
        const unsigned total_winners_gens_winners_only = _total_winners_gens -
                                                         (non_winners *
                                                          optimiser->get_max_gens());
        _avg_winners_gens_winners_only = (double)total_winners_gens_winners_only /
                                         (double)_num_winners;

        std::cout << "Num winners: " << _num_winners << "/" << num_runs << std::endl;
        std::cout << "Average winner generation: " << _avg_winners_gens << std::endl;
        std::cout << "Average winner generation (only winners): " <<
            _avg_winners_gens_winners_only << std::endl;
        std::cout << "Duration: " << duration << " seconds" << std::endl;

    }

    const std::optional<std::string>& get_exp_dir_path() const
    {
        return _exp_dir_path;
    }

    std::vector<std::string> get_run_dir_paths() const
    {
        return collect_dirs_in(_exp_dir_path.value());
    }

    void set_domain_trace(const bool domain_trace)
    {
        _domain->set_trace(domain_trace);
    }

    void set_domain_seed(const unsigned seed)
    {
        _domain->set_seed(seed);
    }

    double get_avg_winners_gens() const
    {
        return _avg_winners_gens;
    }

    double get_avg_winners_gens_winners_only() const
    {
        return _avg_winners_gens_winners_only;
    }

    unsigned get_num_winners() const
    {
        return _num_winners;
    }

private:

    static void run(std::shared_ptr<Domain<G, T>> m_domain,
                    std::shared_ptr<Optimiser<G, T>> a_optimiser,
                    std::shared_ptr<GPMap<G, T>> m_gp_map,
                    const unsigned run_num,
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
        domain->exp_run_reset(run_num);

        //Copy and reset optimiser
        std::unique_ptr<Optimiser<G, T>> optimiser = a_optimiser->clone();
        optimiser->reset();

        //Create new data collector
        DataCollector<G, T> data_collector(exp_dir_path, optimiser->get_max_gens(),
                                           dump_winners_only, trace);

        //Call optimiser
        const bool optimiser_status = optimiser->optimise(domain, m_gp_map,
                                                          data_collector);

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

    std::shared_ptr<Domain<G, T>> _domain;
    std::shared_ptr<GPMap<G, T>> _gp_map;

    std::optional<std::string> _exp_dir_path;
    const std::optional<const JSON> _exp_json;
    const bool _dump_data;
    const bool _dump_winners_only;

    unsigned _num_winners;
    unsigned _total_winners_gens;
    double _avg_winners_gens;
    double _avg_winners_gens_winners_only;

};

template <typename G, typename T>
void individual_run(const JSON& json)
{

    Organism<G, T> organism(json.at({"Organism"}));

    auto domain = Factory<Domain<G, T>>::create(json.at({"Domain"}));

    domain->set_render(json.value({"IndividualRun", "render"}, false));
    domain->set_trace(json.value({"IndividualRun", "domain_trace"}, false));
    std::optional<unsigned> domain_run_seed = std::nullopt;
    if(json.has_value({"Domain", "seed"}))
        domain_run_seed = json.at({"Domain", "seed"});

    domain->exp_run_reset(0, true);

    double fitness = domain->evaluate_org(
                         organism,
                         json.value({"IndividualRun", "num_trials"}, 1)
                     );

    std::cout << "Individual run fitness: " << fitness << std::endl;

}

} // namespace NeuroEvo

#endif
