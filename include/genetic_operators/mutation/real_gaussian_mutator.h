#ifndef _REAL_GAUSSIAN_MUTATOR_H_
#define _REAL_GAUSSIAN_MUTATOR_H_

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
