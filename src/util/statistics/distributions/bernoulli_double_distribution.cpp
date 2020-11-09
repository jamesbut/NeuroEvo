#include <util/statistics/distributions/bernoulli_double_distribution.h>

namespace NeuroEvo {

BernoulliDoubleDistribution::BernoulliDoubleDistribution(const double p) :
    _bern_distr(p) {}

double BernoulliDoubleDistribution::next()
{
    return static_cast<double>(_bern_distr.next());
}

void BernoulliDoubleDistribution::reset()
{
    _bern_distr.reset();
}

} // namespace NeuroEvo
