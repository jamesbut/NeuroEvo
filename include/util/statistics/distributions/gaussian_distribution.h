#ifndef _GAUSSIAN_DISTRIBUTION_H_
#define _GAUSSIAN_DISTRIBUTION_H_

#include <util/statistics/distributions/distribution.h>

namespace NeuroEvo {

class GaussianDistribution : public Distribution<double> {

public:

    GaussianDistribution(const double mean = 0., const double stddev = 1.,
                         const std::optional<unsigned> seed = std::nullopt);

    double next() override;

private:

    void reset() override;
    GaussianDistribution* clone_impl() const override;
    JSON to_json_impl() const override;

    std::normal_distribution<double> _distr;

};

} // namespace NeuroEvo

#endif
