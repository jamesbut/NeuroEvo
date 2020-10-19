#ifndef _REAL_GAUSSIAN_MUTATOR_H_
#define _REAL_GAUSSIAN_MUTATOR_H_

#include <genetic_operators/mutation/mutator.h>
#include <util/statistics/distributions/gaussian_distribution.h>

namespace NeuroEvo {

class RealGaussianMutator : public Mutator<double> {

public:

    RealGaussianMutator(const double mutation_rate, const double mutation_power);

    double mutate_gene(double gene) override;

private:

    void reset_mutator(const std::optional<unsigned>& seed) override;

    RealGaussianMutator* clone_impl() const override
    {
        return new RealGaussianMutator(*this);
    }

    GaussianDistribution _mut_power_distr;

};


} // namespace NeuroEvo

#endif
