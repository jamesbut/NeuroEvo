#ifndef _ROULETTE_WHEEL_SELECTION_H_
#define _ROULETTE_WHEEL_SELECTION_H_

/*
    This is a standard roulette wheel selection that additonally
    accounts for negative fitness values. Therefore, it scales
    all the fitness to above 0. This creates slight problems in
    that the lowest fitness organism gets 0% of the 'pie'. One can
    include a bias term to give it some proportion of the selection.
*/

#include <genetic_operators/selection/selection.h>
#include <random>

class RouletteWheelSelection : public Selection {

public:

    RouletteWheelSelection(const double BIAS = 1);

    //Selects genome according to roulette wheel selection
    Organism select(std::vector<Organism>& orgs) override;

private:

    std::vector<double> scale_fitnesses(std::vector<Organism>& orgs);
    double calculate_total_pop_fitness(std::vector<double>& fitnesses);

    //Bias for scaling fitness
    const double _BIAS;

    //RNG
    std::mt19937 _rng_generator;
    std::uniform_real_distribution<> _rng_uniform_distr;

};

#endif
