#ifndef _RUN_SCHEDULER_
#define _RUN_SCHEDULER_

/*
 * The purpose of this class is to run a number of experimental runs in parallel. It does this 
 * by monitoring which threads are running and, if they are finished, another job will be 
 * assigned to that thread.
 */

#include <thread>
#include <util/concurrency/run_args.h>

namespace NeuroEvo {

template <typename G, typename T>
class RunScheduler {

public: 

   RunScheduler(const RunArguments<G, T>& run_arguments, 
                const unsigned num_runs,
                const unsigned max_num_threads = std::thread::hardware_concurrency() - 1) :
        _run_args(run_arguments),
        _num_runs(num_runs),
        _max_num_threads(max_num_threads) {}

   //Dispatches jobs - blocks until finished
   //void dispatch(void (&run)(const unsigned, const unsigned, bool&))
   void dispatch(void (&run)(const unsigned, 
                             const unsigned, 
                             Selection<G, T>*,
                             Domain<G, T>*,
                             GenotypeSpec<G>*,
                             GPMap<G, T>*,
                             const std::optional<const std::string>&,
                             unsigned&,
                             bool&,
                             const bool,
                             const unsigned,
                             const bool))

   {

        std::vector<std::thread> threads(_max_num_threads);

        //Must initialise array on heap, it seems to be the only option here
        bool* finished_flags = new bool[_max_num_threads];
        for(unsigned i = 0; i < _max_num_threads; i++)
            finished_flags[i] = true;

        unsigned num_runs_started = 0;
        unsigned num_runs_completed = 0;
        std::vector<std::size_t> available_thread_indices;
        available_thread_indices.reserve(_max_num_threads);

        while(num_runs_completed < _num_runs)
        {

            //std::this_thread::sleep_for(std::chrono::milliseconds(100));    

            std::cout << "Started: " << num_runs_started << std::endl;
            std::cout << "Finished: " << num_runs_completed << std::endl;
            std::cout << "Finished flags: ";
            for(unsigned i = 0; i < _max_num_threads; i++)
                std::cout << finished_flags[i] << " ";
            std::cout << std::endl;

            //Monitor for available threads
            std::optional<unsigned> available_thread_index = std::nullopt;
            for(std::size_t i = 0; i < _max_num_threads; i++)
                if(finished_flags[i])
                {
                    available_thread_index = i;
                    break;
                }
                
            
            for(std::size_t i = 0; i < _max_num_threads; i++)
                if(finished_flags[i])
                    available_thread_indices.push_back(i);
                    

            //If there are available threads 
            if(available_thread_index.has_value())
            //if(!available_thread_indices.empty())
            {
                //Join previously spawned thread
                
                if(threads[available_thread_index.value()].joinable()) 
                {
                    threads[available_thread_index.value()].join();
                    num_runs_completed++;
                }
                

                //Join previously spawned threads
                
                for(std::size_t i = 0; i < available_thread_indices.size(); i++)
                    if(threads[available_thread_indices[i]].joinable())
                    {
                        //threads[available_thread_indices[i]].join();
                        //num_runs_completed++;
                    }
               

                //If there are more runs to be processed, spawn new thread
                if(num_runs_started < _num_runs)
                {
                    //const std::size_t thread_index = available_thread_indices.front();
                    const std::size_t thread_index = available_thread_index.value();

                    finished_flags[thread_index] = false;

                    /*
                    threads[available_thread_index.value()] = std::thread(
                            run, num_runs_started, 
                            wait_times[num_runs_started], 
                            std::ref(finished_flags[available_thread_index.value()]));
                    */

                    std::cout << "Starting run: " << num_runs_started << std::endl;

                    threads[thread_index] = std::thread(
                            run, _run_args.pop_size, _run_args.max_gens, _run_args.selector,
                            _run_args.domain, _run_args.geno_spec, _run_args.gp_map,
                            _run_args.exp_dir_path, std::ref(_run_args.num_winners),
                            std::ref(finished_flags[thread_index]),
                            _run_args.trace, _run_args.num_trials, _run_args.domain_parallel);
                    
                    num_runs_started++;

                }

            }

        }

        delete[] finished_flags;

   }

private:

   const RunArguments<G, T> _run_args;
   const unsigned _num_runs;
   const unsigned _max_num_threads;

};

} // namespace NeuroEvo

#endif
