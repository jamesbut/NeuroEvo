#ifndef _REAL_GAUSSIAN_MUTATOR_H_
#define _REAL_GAUSSIAN_MUTATOR_H_

#include <genetic_operators/mutation/mutator.h>
#include <util/statistics/distributions/gaussian_distribution.h>

namespace NeuroEvo {

class RealGaussianMutator : public Mutator<double> {

public:

    RealGaussianMutator(
        const double mutation_rate,
        const double mutation_power,
        const std::optional<const double> lower_bound = std::nullopt,
        const std::optional<const double> upper_bound = std::nullopt);

    //A vector of bounds can be given such that each individual gene has a different
    //set of bounds
    //This vector must be the same size as the genotype otherwise an exception
    //will be thrown when trying to mutate
    RealGaussianMutator(const double mutation_rate, const double mutation_power,
                        const std::vector<double>& lower_bounds,
                        const std::vector<double>& upper_bounds);

    RealGaussianMutator(const JSON& json);

    void mutate(std::vector<double>& genes) override;

private:

    void reset_mutator(const std::optional<unsigned>& seed) override;

    RealGaussianMutator* clone_impl() const override
    {
        return new RealGaussianMutator(*this);
    }

    GaussianDistribution _mut_power_distr;

    //TODO: Subsume into 2 variables not 4

    //Will not mutate genes beyond bounds if bounds are given
    //If optional is not set, no bounds are used
    //If double is used for bounds, the same bound is used for all gene values
    //otherwise different bounds are used for each individual gene
    const std::optional<const double> _lower_bound;
    const std::optional<const double> _upper_bound;
    const std::optional<const std::vector<double>> _lower_bounds;
    const std::optional<const std::vector<double>> _upper_bounds;

};


} // namespace NeuroEvo

#endif
