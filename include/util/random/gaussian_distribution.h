#ifndef _GAUSSIAN_DISTRIBUTION_H_
#define _GAUSSIAN_DISTRIBUTION_H_

#include <utils/random/distribution.h>

class GaussianDistribution : public Distribution<double> {

public:

    GaussianDistribution(const double mean, const double stddev,
                         const std::optional<unsigned> seed = std::nullopt);

    double next() override;

private:

    std::normal_distribution<double> _distr;

};

#endif
