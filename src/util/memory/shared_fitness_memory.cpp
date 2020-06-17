#include <util/memory/shared_fitness_memory.h>
#include <iostream>

namespace NeuroEvo {

SharedFitnessMemory::SharedFitnessMemory(const unsigned pop_size, const unsigned num_runs) :
    _num_runs(num_runs),
    _memory(pop_size * num_runs, "/fitness_shared_memory") {}

void SharedFitnessMemory::write_fitness(const double fitness, const unsigned individual, 
                                        const unsigned run) 
{

    try 
    {
        _memory.write_data(fitness, individual * _num_runs + run);
    } catch (std::out_of_range e) 
    {
        std::cout << e.what() << std::endl;
        std::cout << "Fitness not set!" << std::endl;
    }

}

double SharedFitnessMemory::get_fitness(const unsigned individual, const unsigned run) {

    double fitness = 0;

    try 
    {
        fitness = _memory.read_data(individual * _num_runs + run);
    } catch (std::out_of_range e) 
    {
        std::cout << e.what() << std::endl;
        std::cout << "Returned 0 as fitness!" << std::endl;
    }

    return fitness;

}

} // namespace NeuroEvo
