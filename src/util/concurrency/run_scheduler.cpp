#include <util/concurrency/run_scheduler.h>
#include <vector>
#include <iostream>
#include <optional>

namespace NeuroEvo {

RunScheduler::RunScheduler(const RunArguments& run_arguments, 
                           const unsigned num_runs,
                           const unsigned max_num_threads) :
    _run_args(run_arguments),
    _num_runs(num_runs),
    _max_num_threads(max_num_threads) {}

void RunScheduler::dispatch(void (&run)(const unsigned, const unsigned, bool&))
{

    std::vector<std::thread> threads(_max_num_threads);
    std::vector<unsigned> wait_times{10, 1, 2};

    //Must initialise array on heap, it seems to be the only option here
    bool* finished_flags = new bool[_max_num_threads];
    for(unsigned i = 0; i < _max_num_threads; i++)
        finished_flags[i] = true;

    unsigned num_runs_started = 0;
    unsigned num_runs_completed = 0;

    while(num_runs_completed < _num_runs)
    {

        //Monitor for available threads
        std::optional<unsigned> available_thread_index = std::nullopt;
        for(unsigned i = 0; i < _max_num_threads; i++)
        {
            if(finished_flags[i])
            {
                available_thread_index = i;
                break;
            }
        }

        //If there are available threads 
        if(available_thread_index.has_value())
        {
            //Join previously spawned thread
            if(threads[available_thread_index.value()].joinable()) 
            {
                threads[available_thread_index.value()].join();
                num_runs_completed++;
            }

            //If there are more runs to be processed, spawn new thread
            if(num_runs_started < _num_runs)
            {

                finished_flags[available_thread_index.value()] = false;

                threads[available_thread_index.value()] = std::thread(
                        run, num_runs_started, 
                        wait_times[num_runs_started], 
                        std::ref(finished_flags[available_thread_index.value()]));
                
                num_runs_started++;

            }

        }

    }

}

} // namespace NeuroEvo
