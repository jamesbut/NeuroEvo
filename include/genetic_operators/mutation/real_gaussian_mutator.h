#ifndef _REAL_GAUSSIAN_MUTATOR_H_
#define _REAL_GAUSSIAN_MUTATOR_H_

#include <genetic_operators/mutation/mutator.h>
#include <util/random/gaussian_distribution.h>

namespace NeuroEvo {

class RealGaussianMutator : public Mutator<double> {

public:

    RealGaussianMutator(const double mutation_rate, const double mutation_power);

    double mutate_gene(double gene) override;

private:

    GaussianDistribution _mut_power_distr;

};


} // namespace NeuroEvo

#endif
