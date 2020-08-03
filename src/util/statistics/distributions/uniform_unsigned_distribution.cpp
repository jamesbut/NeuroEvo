#include <util/statistics/distributions/uniform_unsigned_distribution.h>

namespace NeuroEvo {

UniformUnsignedDistribution::UniformUnsignedDistribution(const unsigned long lower_bound,
                                                         const unsigned long upper_bound,
                                                         const std::optional<unsigned> seed) :
    Distribution<unsigned long>(seed),
    _distr(lower_bound, upper_bound) {}

unsigned long UniformUnsignedDistribution::next() 
{
    return _distr(_rng);
}

unsigned long UniformUnsignedDistribution::get(const unsigned long lower_bound,
                                               const unsigned long upper_bound) 
{
    UniformUnsignedDistribution distr(lower_bound, upper_bound);
    return distr.next();
}

void UniformUnsignedDistribution::reset() 
{
    _distr.reset();
}

}
