#include <genetic_operators/mutation/real_gaussian_mutator.h>
#include <stdexcept>

namespace NeuroEvo {

RealGaussianMutator::RealGaussianMutator(
    const double mutation_rate,
    const double mutation_power) :
    Mutator(mutation_rate),
    _mut_power_distr(0, mutation_power),
    _lower_bound(std::nullopt),
    _upper_bound(std::nullopt),
    _lower_bounds(std::nullopt),
    _upper_bounds(std::nullopt) {}

RealGaussianMutator::RealGaussianMutator(
    const double mutation_rate,
    const double mutation_power,
    const double lower_bound,
    const double upper_bound) :
    Mutator(mutation_rate),
    _mut_power_distr(0, mutation_power),
    _lower_bound(lower_bound),
    _upper_bound(upper_bound),
    _lower_bounds(std::nullopt),
    _upper_bounds(std::nullopt) {}

RealGaussianMutator::RealGaussianMutator(
    const double mutation_rate,
    const double mutation_power,
    const std::vector<double>& lower_bounds,
    const std::vector<double>& upper_bounds) :
    Mutator(mutation_rate),
    _mut_power_distr(0, mutation_power),
    _lower_bound(std::nullopt),
    _upper_bound(std::nullopt),
    _lower_bounds(lower_bounds),
    _upper_bounds(upper_bounds)
    {
        if(_lower_bounds->size() != _upper_bounds->size())
            throw std::length_error(
                "Lower and upper bounds vectors must be the same size "
                "when given to RealGaussianMutator constructor");
    }

void RealGaussianMutator::mutate(std::vector<double>& genes) {

    for(std::size_t i = 0; i < genes.size(); i++)
    {
        if(!should_mutate())
            continue;

        //Add value from gaussian distribution
        double new_gene = genes[i] + _mut_power_distr.next();

        //Check for lower bounds
        if(_lower_bound.has_value())
            if(new_gene < _lower_bound.value())
                new_gene = _lower_bound.value();

        if(_lower_bounds.has_value())
        {
            //Check for lower bounds size and compare to genes size
            //I do not need to check size for upper bounds too because it has
            //already been checked in the constructor that both bounds are the same
            //size
            if(_lower_bounds->size() != genes.size())
                throw std::length_error("Genes size and lower bounds size mismatch"
                    " in RealGaussianMutator::mutate\nLower bounds size: "
                    + std::to_string(_lower_bounds->size()) + " Genes size: "
                    + std::to_string(genes.size()));

            if(new_gene < _lower_bounds.value()[i])
                new_gene = _lower_bounds.value()[i];

        }

        //Check for upper bounds
        if(_upper_bound.has_value())
            if(new_gene > _upper_bound.value())
                new_gene = _upper_bound.value();

        if(_upper_bounds.has_value())
            if(new_gene > _upper_bounds.value()[i])
                new_gene = _upper_bounds.value()[i];

        genes[i] = new_gene;
    }
}

void RealGaussianMutator::reset_mutator(const std::optional<unsigned>& seed)
{
    if(seed.has_value())
        _mut_power_distr.set_seed(seed.value());
    else
        _mut_power_distr.randomly_seed();
}

} // namespace NeuroEvo
