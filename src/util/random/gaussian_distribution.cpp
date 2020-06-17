#include <util/random/gaussian_distribution.h>

namespace NeuroEvo {

GaussianDistribution::GaussianDistribution(const double mean, const double stddev,
                                           const std::optional<unsigned> seed) :
    Distribution(seed),
    _distr(mean, stddev) {}

double GaussianDistribution::next() {
    return _distr(_rng);
}

} // namespace NeuroEvo
