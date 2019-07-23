#ifndef _UNIFORM_SELECTION_H_
#define _UNIFORM_SELECTION_H_

/*
    Uniform selection chooses an organism randomly,
    it does not care about the fitness.
*/

#include <genetic_operators/selection/selection.h>
#include <random>

class UniformSelection : public Selection {

public:

    UniformSelection();

    Organism select(std::vector<Organism>& population) override;

private:

    // RNG
    std::mt19937 _rng_generator;
    std::uniform_real_distribution<> _rng_uniform_distr;

};

#endif
