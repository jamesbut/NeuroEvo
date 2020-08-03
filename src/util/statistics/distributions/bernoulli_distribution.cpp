#include <util/statistics/distributions/bernoulli_distribution.h>

namespace NeuroEvo {

BernoulliDistribution::BernoulliDistribution(const double p) :
    _distr(p) {}

bool BernoulliDistribution::next() 
{
    return _distr(_rng);
}

void BernoulliDistribution::reset()
{
    _distr.reset();
}

} // namespace NeuroEvo
