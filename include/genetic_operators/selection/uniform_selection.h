#ifndef _UNIFORM_SELECTION_H_
#define _UNIFORM_SELECTION_H_

/*
    Uniform selection chooses an organism randomly,
    it does not care about the fitness.
*/

#include <genetic_operators/selection/selection.h>
#include <random>

namespace NeuroEvo {

template <typename G>
class UniformSelection : public Selection<G> 
{

public:

    UniformSelection() :
        _uniform_distr(0., 1.) {}

    Organism<G> select(std::vector<Organism<G>>& population) override 
    {

        const double rand_num = _uniform_distr.next();

        int chosen_org = floor(rand_num * orgs.size());

        return Organism(orgs.at(chosen_org).get_phenotype_spec(),
                        orgs.at(chosen_org).get_genotype(),
                        orgs.at(chosen_org).get_gp_map_spec());

    }

private:

    UniformRealDistribution _uniform_distr;

};

} // namespace NeuroEvo

#endif
