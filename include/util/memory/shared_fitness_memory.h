#ifndef _SHARED_FITNESS_MEMORY_H_
#define _SHARED_FITNESS_MEMORY_H_

/*
    A shared memory class for the storage of fitness scores for
    a population of individuals being tested for a number of trials
*/

#include <util/memory/shared_memory.h>

namespace NeuroEvo {

class SharedFitnessMemory {

public:

    SharedFitnessMemory(const unsigned pop_size, const unsigned num_runs);

    void write_fitness(const double fitness, const unsigned individual, const unsigned run);
    double get_fitness(const unsigned individual, const unsigned run);

private:

    const unsigned _num_runs;

    SharedMemory<double> _memory;

};

} // namespace NeuroEvo

#endif
