#ifndef _UNIFORM_DISTRIBUTION_H_
#define _UNIFORM_DISTRIBUTION_H_

#include <util/random/distribution.h>

namespace NeuroEvo {
namespace Utils {

class UniformDistribution : public Distribution {

public:

    UniformDistribution(const double lower_bound, const double upper_bound) :
        _distr(lower_bound, upper_bound),
        Distribution() {}

    UniformDistribution(const double lower_bound, const double upper_bound,
                        const unsigned seed) :
        _distr(lower_bound, upper_bound),
        Distribution(seed) {}

    double next() override {

        return _distr(_rng);

    }

private:

    std::uniform_real_distribution<> _distr;

};

} // namespace Utils
} // namespace NeuroEvo

#endif
