#include <util/memory/shared_fitness_memory.h>
#include <iostream>

namespace NeuroEvo {
namespace Utils {

SharedFitnessMemory::SharedFitnessMemory(const int POP_SIZE, const int NUM_RUNS) :
    _POP_SIZE(POP_SIZE),
    _NUM_RUNS(NUM_RUNS),
    _memory(POP_SIZE * NUM_RUNS, "/fitness_shared_memory") {}

void SharedFitnessMemory::write_fitness(const double fitness, const int individual, const int run) {

    try {

        _memory.write_data(fitness, individual * _NUM_RUNS + run);

    } catch (std::out_of_range e) {

        std::cout << e.what() << std::endl;
        std::cout << "Fitness not set!" << std::endl;

    }

}

double SharedFitnessMemory::get_fitness(const int individual, const int run) {

    double fitness = 0;

    try {

        fitness = _memory.read_data(individual * _NUM_RUNS + run);

    } catch (std::out_of_range e) {

        std::cout << e.what() << std::endl;
        std::cout << "Returned 0 as fitness!" << std::endl;

    }

    return fitness;

}

} // namespace Utils
} // namespace NeuroEvo
