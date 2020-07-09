#ifndef _TRUNCATION_SELECTION_H_
#define _TRUNCATION_SELECTION_H_

/*
    Truncation selection randomly selects from the
    top percentage performers of the population.
*/

#include <genetic_operators/selection/selection.h>
#include <random>

namespace NeuroEvo {

template <typename G, typename T>
class TruncationSelection : public Selection<G, T>
{

public:

    // Percentage selection is given as a decimal between 0 and 1
    TruncationSelection(const double percentage_selection) :
        _percentage_selection(percentage_selection),
        _uniform_distr(0., 1.) {}

    Organism<G, T> select(const std::vector<Organism<G, T>>& orgs) override 
    {

        // Get vector of sorted population indices
        std::vector<int> sorted_pop_indices = sort_population(orgs);

        // Only consider the top performers
        const int num_orgs_considered = floor(_percentage_selection * orgs.size());
        std::vector<int> considered_indices(&sorted_pop_indices[0], 
                                            &sorted_pop_indices[num_orgs_considered]);

        // Choose uniformly amongst the top performers
        const double rand_num = _uniform_distr.next();
        const int chosen_org = considered_indices[floor(rand_num * considered_indices.size())];

        //Create copy of this org and return
        return Organism(orgs.at(chosen_org).get_genotype(),
                        orgs.at(chosen_org).get_phenotype_spec(),
                        orgs.at(chosen_org).get_gp_map());

    }

private:

    std::vector<int> sort_population(const std::vector<Organism<G, T>>& orgs) 
    {

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

    // The amount of the population that is selected to
    // reproduce
    const double _percentage_selection;

    UniformRealDistribution _uniform_distr;

};

} // namespace NeuroEvo

#endif
