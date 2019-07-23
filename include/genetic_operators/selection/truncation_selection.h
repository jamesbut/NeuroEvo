#ifndef _TRUNCATION_SELECTION_H_
#define _TRUNCATION_SELECTION_H_

/*
    Truncation selection randomly selects from the
    top percentage performers of the population.
*/

#include <genetic_operators/selection/selection.h>
#include <random>

class TruncationSelection : public Selection {

public:

    // Percentage selection is given as a decimal between 0 and 1
    TruncationSelection(const double PERCENTAGE_SELECTION);

    Organism select(std::vector<Organism>& orgs) override;

private:

    std::vector<int> sort_population(std::vector<Organism>& population);

    // The amount of the population that is selected to
    // reproduce
    const double _PERCENTAGE_SELECTION;

    // RNG
    std::mt19937 _rng_generator;
    std::uniform_real_distribution<> _rng_uniform_distr;

};

#endif
