#ifndef _UNIFORM_DISTRIBUTION_H_
#define _UNIFORM_DISTRIBUTION_H_

/*
    A uniform distribution over reals
*/

#include <util/random/distribution.h>

namespace NeuroEvo {
namespace Utils {

class UniformRealDistribution : public Distribution<double> {

public:

    UniformRealDistribution(const double lower_bound, const double upper_bound,
                            const std::optional<unsigned> seed = std::nullopt);

    double next() override;

private:

    std::uniform_real_distribution<> _distr;

};

} // namespace Utils
} // namespace NeuroEvo

#endif
