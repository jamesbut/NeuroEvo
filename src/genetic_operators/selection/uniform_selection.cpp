#include <genetic_operators/selection/uniform_selection.h>

namespace NeuroEvo {
namespace Selectors {

UniformSelection::UniformSelection() :
    _rng_generator(std::random_device()()),
    _rng_uniform_distr(0, 1) {}

// Choose organism completely randomly with each individual of the
// population having an equal chance of getting chosen
Organism UniformSelection::select(std::vector<Organism>& orgs) {

    double rand_num = _rng_uniform_distr(_rng_generator);

    int chosen_org = floor(rand_num * orgs.size());

    return Organism(orgs.at(chosen_org).get_phenotype_spec(),
                    orgs.at(chosen_org).get_genotype(),
                    orgs.at(chosen_org).get_gp_map_spec());

}

} // namespace Selectors
} // namespace NeuroEvo
