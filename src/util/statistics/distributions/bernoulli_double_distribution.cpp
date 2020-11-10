#include <util/statistics/distributions/bernoulli_double_distribution.h>

namespace NeuroEvo {

BernoulliDoubleDistribution::BernoulliDoubleDistribution(const double p) :
    _distr(p) {}

double BernoulliDoubleDistribution::next()
{
    return static_cast<double>(_distr(_rng));
}

void BernoulliDoubleDistribution::reset()
{
    _distr.reset();
}

} // namespace NeuroEvo
