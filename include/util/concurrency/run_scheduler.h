#ifndef _RUN_SCHEDULER_
#define _RUN_SCHEDULER_

/*
 * The purpose of this class is to run a number of experimental runs in parallel. It does this 
 * by monitoring which threads are running and, if they are finished, another job will be 
 * assigned to that thread.
 */

#include <thread>

namespace NeuroEvo {

class RunScheduler {

public: 

    struct RunArguments {
        unsigned pop_size;
    };

   RunScheduler(const RunArguments& run_arguments, 
                const unsigned num_runs,
                const unsigned max_num_threads = std::thread::hardware_concurrency());

   //Dispatches jobs - blocks until finished
   void dispatch(void (&run)(const unsigned, const unsigned, bool&));

private:

   const RunArguments _run_args;
   const unsigned _num_runs;
   const unsigned _max_num_threads;

};

} // namespace NeuroEvo

#endif
