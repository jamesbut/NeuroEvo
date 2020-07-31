#include <util/statistics/distributions/uniform_real_distribution.h>

namespace NeuroEvo {

UniformRealDistribution::UniformRealDistribution(const double lower_bound, 
                                                 const double upper_bound,
                                                 const std::optional<unsigned> seed) :
    Distribution(seed),
    _distr(lower_bound, upper_bound) {}

double UniformRealDistribution::next() 
{
    return _distr(_rng);
}

} // namespace NeuroEvo
