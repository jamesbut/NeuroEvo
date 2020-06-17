#include <genetic_operators/mutation/real_gaussian_mutator.h>

namespace NeuroEvo {

RealGaussianMutator::RealGaussianMutator(const double mutation_rate, 
                                         const double mutation_power) :
    Mutator(mutation_rate),
    _mut_power_distr(0, mutation_power) {}

double RealGaussianMutator::mutate_gene(double gene) {
    return gene += _mut_power_distr.next();
}

} // namespace NeuroEvo
