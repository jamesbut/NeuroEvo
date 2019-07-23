#include <genetic_operators/selection/truncation_selection.h>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <iostream>

TruncationSelection::TruncationSelection(const double PERCENTAGE_SELECTION) :
    _PERCENTAGE_SELECTION(PERCENTAGE_SELECTION),
    _rng_generator(std::random_device()()),
    _rng_uniform_distr(0, 1) {}

Organism TruncationSelection::select(std::vector<Organism>& orgs) {

    // Get vector of sorted population indices
    std::vector<int> sorted_pop_indices = sort_population(orgs);

    // Only consider the top performers
    const int NUM_ORGS_CONSIDERED = floor(_PERCENTAGE_SELECTION * orgs.size());
    std::vector<int> considered_indices(&sorted_pop_indices[0], &sorted_pop_indices[NUM_ORGS_CONSIDERED]);

    // Choose uniformly amongst the top performers
    double rand_num = _rng_uniform_distr(_rng_generator);
    int chosen_org = considered_indices[floor(rand_num * considered_indices.size())];

    //Create copy of this org and return
    return Organism(orgs.at(chosen_org).get_phenotype_spec(),
                    orgs.at(chosen_org).get_genotype(),
                    orgs.at(chosen_org).get_gp_map());

}

// Returns an ordered vector of indices, describing the fitness ordering of the population
// This is so inefficient because it sorts the population for every selection
// TODO: Make this more efficient
std::vector<int> TruncationSelection::sort_population(std::vector<Organism>& orgs) {

    // Create vector of unsorted indices
    std::vector<int> sorted_indices(orgs.size());
    std::iota(begin(sorted_indices), end(sorted_indices), 0);

    // Sort indices by population fitness
    std::sort(
        begin(sorted_indices), end(sorted_indices),
        [&](int i1, int i2) { return orgs.at(i1).get_fitness() > orgs.at(i2).get_fitness(); }
    );

    return sorted_indices;

}
