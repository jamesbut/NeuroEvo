#ifndef _SHARED_FITNESS_MEMORY_H_
#define _SHARED_FITNESS_MEMORY_H_

/*
    A shared memory class for the storage of fitness scores for
    a population of individuals being tested for a number of trials
*/

#include <util/memory/shared_memory.h>

class SharedFitnessMemory {

public:

    SharedFitnessMemory(const int POP_SIZE, const int NUM_RUNS);

    void write_fitness(const double fitness, const int individual, const int run);
    double get_fitness(const int individual, const int run);

private:

    const int _POP_SIZE;
    const int _NUM_RUNS;

    SharedMemory<double> _memory;

};

#endif
