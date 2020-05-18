#include <util/random/uniform_real_distribution.h>

namespace NeuroEvo {
namespace Utils {

UniformRealDistibution::UniformRealDistribution(const double lower_bound, 
                                                const double upper_bound,
                                                const std::optional<unsigned> seed) :
    _distr(lower_bound, upper_bound),
    Distribution(seed) {}

double UniformRealDistribution::next() {
    return _distr(_rng);
}

} // namespace Utils
} // namespace NeuroEvo
