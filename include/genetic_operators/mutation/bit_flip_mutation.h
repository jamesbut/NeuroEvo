#ifndef _BIT_FLIP_MUTATION_H_
#define _BIT_FLIP_MUTATION_H_

/*
    This bit flip mutation works on vectors with values
    spanning the range 0 and 1. It checks to see whether
    the value is less than or greater than 0.5 and then
    changes the value to 1 or 0 respectively.
*/

#include <genetic_operators/mutation/mutation.h>
#include <random>

namespace NeuroEvo {
namespace Mutators {

class BitFlipMutation : public Mutation {

public:

    BitFlipMutation(const double MUTATION_RATE, Constraint* constraint = nullptr);

    void mutate(std::vector<double>& vector) override;

private:

    const double _MUTATION_RATE;

    //RNG
    std::mt19937 _rng_generator;
    std::uniform_real_distribution<> _rng_uniform_distr;

};

} // namespace Mutators
} // namespace NeuroEvo

#endif
