#include <genetic_operators/mutation/real_gaussian_mutation.h>
#include <iostream>

RealGaussianMutation::RealGaussianMutation(const double MUTATION_RATE, const double MUTATION_POWER,
                                           Constraint* constraint) :
    _MUTATION_RATE(MUTATION_RATE),
    _MUTATION_POWER(MUTATION_POWER),
    _rng_generator(std::random_device()()),
    _rng_uniform_distr(0, 1),
    _rng_gaussian_mutation_power_distr(0, _MUTATION_POWER),
    Mutation(constraint) {}

void RealGaussianMutation::mutate(std::vector<double>& vector) {

    for(size_t i = 0; i < vector.size(); i++) {

        if (_rng_uniform_distr(_rng_generator) < _MUTATION_RATE) {

            double ran_num = _rng_gaussian_mutation_power_distr(_rng_generator);
            vector.at(i) += ran_num;

        }

    }

}
