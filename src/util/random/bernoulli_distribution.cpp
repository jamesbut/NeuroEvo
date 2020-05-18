#include <util/random/bernoulli_distribution.h>

namespace NeuroEvo {
namespace Utils {

BernoulliDistribution::BernoulliDistribution(const double p) :
    _distr(p) {}

bool BernoulliDistribution::next() {
    return _distr(_rng);
}

} // namespace Utils
} // namespace NeuroEvo
