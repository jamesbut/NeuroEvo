#ifndef _REAL_GAUSSIAN_MUTATOR_H_
#define _REAL_GAUSSIAN_MUTATOR_H_

#include <genetic_operators/mutation/mutator.h>
#include <util/statistics/distributions/gaussian_distribution.h>

namespace NeuroEvo {

class RealGaussianMutator : public Mutator<double> {

public:

    RealGaussianMutator(const double mutation_rate, const double mutation_power);

    RealGaussianMutator(const double mutation_rate, const double mutation_power,
                        const double lower_bound, const double upper_bound);

    //A vector of bounds can be given such that each individual gene has a different
    //set of bounds
    //This vector must be the same size as the genotype otherwise an exception
    //will be thrown when trying to mutate
    RealGaussianMutator(const double mutation_rate, const double mutation_power,
                        const std::vector<const double>& lower_bounds,
                        const std::vector<const double>& upper_bounds);

    void mutate(std::vector<double>& genes) override;

private:

    void reset_mutator(const std::optional<unsigned>& seed) override;

    RealGaussianMutator* clone_impl() const override
    {
        return new RealGaussianMutator(*this);
    }

    GaussianDistribution _mut_power_distr;

    //Will not mutate genes beyond bounds if bounds are given
    //If optional is not set, no bounds are used
    //If double is used for bounds, the same bound is used for all gene values
    //otherwise different bounds are used for each individual gene
    const std::optional<const double> _lower_bound;
    const std::optional<const double> _upper_bound;
    const std::optional<const std::vector<const double>> _lower_bounds;
    const std::optional<const std::vector<const double>> _upper_bounds;

};


} // namespace NeuroEvo

#endif
