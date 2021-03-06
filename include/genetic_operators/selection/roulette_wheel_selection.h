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

namespace NeuroEvo {

template <typename G, typename T>
class RouletteWheelSelection : public Selection<G, T> {

public:

    RouletteWheelSelection(const double bias = 1) :
        _bias(bias),
        _uniform_distr(0., 1.) {}

    //Selects genome according to roulette wheel selection
    Organism<G, T> select(const std::vector<Organism<G, T>>& orgs) override 
    {
        
        //Scale fitnesses
        std::vector<double> scaled_fitnesses = scale_fitnesses(orgs);

        double total_fitness = calculate_total_pop_fitness(scaled_fitnesses);

        double fitness_so_far = 0.0;
        double rand_num = _uniform_distr.next();

        unsigned chosen_org = 0;

        for(size_t i = 0; i < scaled_fitnesses.size(); i++) {

            fitness_so_far += scaled_fitnesses.at(i) / total_fitness;

            if(rand_num < fitness_so_far) {

                chosen_org = i;
                break;

            }

        }

        return Organism(orgs.at(chosen_org).get_genotype(),
                        orgs.at(chosen_org).get_gp_map());

    }

private:

    std::vector<double> scale_fitnesses(const std::vector<Organism<G, T>>& orgs) 
    {

        //Find smallest fitness
        double smallest_fitness;

        for(size_t i = 0; i < orgs.size(); i++) {

            if(i==0)
                smallest_fitness = orgs.at(i).get_fitness().value();
            else
                if(orgs.at(i).get_fitness().value() < smallest_fitness)
                    smallest_fitness = orgs.at(i).get_fitness().value();

        }

        //Scale fitnesses by adding (-1)*smallest_fitness to every value
        //and a bias value so there is never a 0 fitness
        std::vector<double> scaled_fitnesses(orgs.size());

        for(size_t i = 0; i < orgs.size(); i++)
            scaled_fitnesses.at(i) = -smallest_fitness + orgs.at(i).get_fitness().value() 
                                     + _bias;

        return scaled_fitnesses;

    }

    double calculate_total_pop_fitness(std::vector<double>& fitnesses) 
    {

        double total_fitness = 0.0;

        for(size_t i = 0; i < fitnesses.size(); i++)
            total_fitness += fitnesses.at(i);

        return total_fitness;

    }

    RouletteWheelSelection* clone_impl() const override
    {
        return new RouletteWheelSelection(*this);
    }

    void reset(const std::optional<unsigned>& seed) override
    {
        if(seed.has_value())
            _uniform_distr.set_seed(seed.value());
        else
            _uniform_distr.randomly_seed();
    }

    //Bias for scaling fitness
    const double _bias;

    //RNG
    UniformRealDistribution _uniform_distr;

};

} // namespace NeuroEvo

#endif
