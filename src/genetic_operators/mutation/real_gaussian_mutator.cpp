#include <genetic_operators/mutation/real_gaussian_mutator.h>

namespace NeuroEvo {

RealGaussianMutator::RealGaussianMutator(
    const double mutation_rate,
    const double mutation_power,
    const std::optional<const double> lower_bound,
    const std::optional<const double> upper_bound) :
    Mutator(mutation_rate),
    _mut_power_distr(0, mutation_power),
    _lower_bound(lower_bound),
    _upper_bound(upper_bound) {}

double RealGaussianMutator::mutate_gene(double gene) {

    //Add value from gaussian distribution
    double new_gene = gene + _mut_power_distr.next();

    //Check for lower and upper bounds
    if(_lower_bound.has_value() && (new_gene < _lower_bound.value()))
        new_gene = _lower_bound.value();
    if(_upper_bound.has_value() && (new_gene > _upper_bound.value()))
        new_gene = _upper_bound.value();

    return new_gene;
}

void RealGaussianMutator::reset_mutator(const std::optional<unsigned>& seed)
{
    if(seed.has_value())
        _mut_power_distr.set_seed(seed.value());
    else
        _mut_power_distr.randomly_seed();
}

} // namespace NeuroEvo
