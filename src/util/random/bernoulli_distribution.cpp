#include <util/random/bernoulli_distribution.h>

namespace NeuroEvo {

BernoulliDistribution::BernoulliDistribution(const double p) :
    _distr(p) {}

bool BernoulliDistribution::next() {
    return _distr(_rng);
}

} // namespace NeuroEvo
