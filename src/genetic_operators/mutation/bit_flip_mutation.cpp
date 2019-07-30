#include <genetic_operators/mutation/bit_flip_mutation.h>
#include <iostream>

namespace NeuroEvo {
namespace Mutators {

BitFlipMutation::BitFlipMutation(const double MUTATION_RATE, Constraint* constraint) :
    _MUTATION_RATE(MUTATION_RATE),
    _rng_generator(std::random_device()()),
    _rng_uniform_distr(0, 1),
    Mutation(constraint) {}

void BitFlipMutation::mutate(std::vector<double>& vector) {

    std::vector<double> proposed_vector(vector);

    for(auto& value : proposed_vector)
        if(_rng_uniform_distr(_rng_generator) < _MUTATION_RATE) {

            if(value < 0.5)
                value = 1.0;
            else
                value = 0.0;

        }

    //Check that constraints are satisfied before applying changes
    if(_constraint) {
        if(_constraint->satisfied(proposed_vector))
            vector = proposed_vector;
    }
    else {
        vector = proposed_vector;
    }

}

} // namespace Mutators
} // namespace NeuroEvo
