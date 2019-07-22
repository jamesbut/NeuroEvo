#ifndef _REAL_GAUSSIAN_MUTATION_H_
#define _REAL_GAUSSIAN_MUTATION_H_

#include <genetic_operators/mutation/mutation.h>
#include <random>

class RealGaussianMutation : public Mutation {

public:

    RealGaussianMutation(const double MUTATION_RATE, const double MUTATION_POWER,
                         Constraint* constraint = nullptr);

    //Mutates a vector
    void mutate(std::vector<double>& vector) override;

private:

    const double _MUTATION_RATE;
    const double _MUTATION_POWER;

    //RNG
    std::mt19937 _rng_generator;
    std::uniform_real_distribution<> _rng_uniform_distr;
    std::normal_distribution<> _rng_gaussian_mutation_power_distr;

};

#endif
