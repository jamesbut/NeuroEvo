#ifndef _GAUSSIAN_DISTRIBUTION_H_
#define _GAUSSIAN_DISTRIBUTION_H_

#include <util/random/distribution.h>

namespace NeuroEvo {

class GaussianDistribution : public Distribution<double> {

public:

    GaussianDistribution(const double mean = 0., const double stddev = 1.,
                         const std::optional<unsigned> seed = std::nullopt);

    double next() override;

private:

    std::normal_distribution<double> _distr;

};

} // namespace NeuroEvo

#endif
