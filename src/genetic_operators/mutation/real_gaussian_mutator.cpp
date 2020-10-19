#include <genetic_operators/mutation/real_gaussian_mutator.h>

namespace NeuroEvo {

RealGaussianMutator::RealGaussianMutator(const double mutation_rate, 
                                         const double mutation_power) :
    Mutator(mutation_rate),
    _mut_power_distr(0, mutation_power) {}

double RealGaussianMutator::mutate_gene(double gene) {
    return gene += _mut_power_distr.next();
}

void RealGaussianMutator::reset_mutator(const std::optional<unsigned>& seed)
{
    if(seed.has_value())
        _mut_power_distr.set_seed(seed.value());
    else
        _mut_power_distr.randomly_seed();
}

} // namespace NeuroEvo
