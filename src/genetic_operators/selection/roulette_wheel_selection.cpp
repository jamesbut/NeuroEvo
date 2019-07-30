/*
    This is a standard roulette wheel selection that additonally
    accounts for negative fitness values. Therefore, it scales
    all the fitness to above 0. This creates slight problems in
    that the lowest fitness organism gets 0% of the 'pie'. One can
    include a bias term to give it some proportion of the selection.
*/

#include <genetic_operators/selection/roulette_wheel_selection.h>
#include <iostream>

namespace NeuroEvo {
namespace Selectors {

RouletteWheelSelection::RouletteWheelSelection(const double BIAS) :
    _BIAS(BIAS),
    _rng_generator(std::random_device()()),
    _rng_uniform_distr(0, 1) {}

Organism RouletteWheelSelection::select(std::vector<Organism>& orgs) {

    //Scale fitnesses
    std::vector<double> scaled_fitnesses = scale_fitnesses(orgs);

    double total_fitness = calculate_total_pop_fitness(scaled_fitnesses);

    double fitness_so_far = 0.0;
    double rand_num = _rng_uniform_distr(_rng_generator);

    int chosen_org;

    for(size_t i = 0; i < scaled_fitnesses.size(); i++) {

        fitness_so_far += scaled_fitnesses.at(i) / total_fitness;

        if(rand_num < fitness_so_far) {

            chosen_org = i;
            break;

        }

    }

    return Organism(orgs.at(chosen_org).get_phenotype_spec(),
                    orgs.at(chosen_org).get_genotype(),
                    orgs.at(chosen_org).get_gp_map());

}

std::vector<double> RouletteWheelSelection::scale_fitnesses(std::vector<Organism>& orgs) {

    //Find smallest fitness
    double smallest_fitness;

    for(size_t i = 0; i < orgs.size(); i++) {

        if(i==0)
            smallest_fitness = orgs.at(i).get_fitness();
        else
            if(orgs.at(i).get_fitness() < smallest_fitness)
                smallest_fitness = orgs.at(i).get_fitness();

    }

    //Scale fitnesses by adding (-1)*smallest_fitness to every value
    //and a bias value so there is never a 0 fitness
    std::vector<double> scaled_fitnesses(orgs.size());

    for(size_t i = 0; i < orgs.size(); i++)
        scaled_fitnesses.at(i) = -smallest_fitness + orgs.at(i).get_fitness() + _BIAS;

    return scaled_fitnesses;

}

double RouletteWheelSelection::calculate_total_pop_fitness(std::vector<double>& fitnesses) {

    double total_fitness = 0.0;

    for(size_t i = 0; i < fitnesses.size(); i++)
        total_fitness += fitnesses.at(i);

    return total_fitness;

}

} // namespace Selectors
} // namespace NeuroEvo
