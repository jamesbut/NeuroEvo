#ifndef _RUN_SCHEDULER_
#define _RUN_SCHEDULER_

/*
 * The purpose of this class is to run a number of experimental runs in parallel. It does this
 * by monitoring which threads are running and, if they are finished, another job will be
 * assigned to that thread.
 */

#include <thread>
#include <util/concurrency/run_args.h>
#include <queue>

namespace NeuroEvo {

template <typename G, typename T>
class RunScheduler {

public:

   RunScheduler(const RunArguments<G, T>& run_arguments,
                const unsigned num_runs,
                const unsigned max_num_threads =
                    std::thread::hardware_concurrency() - 1) :
        _run_args(run_arguments),
        _num_runs(num_runs),
        _max_num_threads(max_num_threads) {}

    //Dispatches jobs - blocks until finished
    void dispatch(void (&run)(std::shared_ptr<Domain<G, T>>,
                              std::shared_ptr<Optimiser<G, T>> optimiser,
                              std::shared_ptr<GPMap<G, T>>,
                              const unsigned,
                              const std::optional<const std::string>,
                              const bool,
                              unsigned&,
                              bool&,
                              unsigned&,
                              const bool,
                              const bool))

    {

            std::vector<std::thread> threads(_max_num_threads);

            //Must initialise array on heap, it seems to be the only option here
            bool* finished_flags = new bool[_max_num_threads];
            for(unsigned i = 0; i < _max_num_threads; i++)
                finished_flags[i] = true;

            unsigned num_runs_started = 0;
            unsigned num_runs_completed = 0;
            std::deque<std::size_t> available_thread_indices;

            while(num_runs_completed < _num_runs)
            {

                for(std::size_t i = 0; i < _max_num_threads; i++)
                    if(finished_flags[i] &&
                       //Also check whether the index is already in the deque before
                       //adding
                       std::find(available_thread_indices.begin(),
                                 available_thread_indices.end(),
                                 i) == available_thread_indices.end())
                        available_thread_indices.push_back(i);

                //If there are available threads
                if(!available_thread_indices.empty())
                {

                    //Get available thread index and pop queue
                    const std::size_t thread_index = available_thread_indices.front();
                    available_thread_indices.pop_front();

                    //Join previously spawned thread
                    if(threads[thread_index].joinable())
                    {
                        threads[thread_index].join();
                        num_runs_completed++;
                    }

                    //If there are more runs to be processed, spawn new thread
                    if(num_runs_started < _num_runs)
                    {
                        finished_flags[thread_index] = false;

                        std::cout << "Starting run: " << num_runs_started << std::endl;

                        threads[thread_index] = std::thread(
                                run,
                                _run_args.domain,
                                std::ref(_run_args.optimiser),
                                _run_args.gp_map,
                                num_runs_started,
                                _run_args.exp_dir_path,
                                _run_args.dump_winners_only,
                                std::ref(_run_args.num_winners),
                                std::ref(finished_flags[thread_index]),
                                std::ref(_run_args.total_winners_gens),
                                _run_args.trace, _run_args.domain_parallel);

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
