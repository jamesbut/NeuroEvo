#ifndef _UNIFORM_REAL_DISTRIBUTION_H_
#define _UNIFORM_REAL_DISTRIBUTION_H_

/*
    A uniform distribution over reals
*/

#include <util/statistics/distributions/distribution.h>

namespace NeuroEvo {

class UniformRealDistribution : public Distribution<double>
{

public:

    UniformRealDistribution(const double lower_bound, const double upper_bound,
                            const std::optional<unsigned> seed = std::nullopt);
    UniformRealDistribution(const JSON& json);

    double next() override;

    //Functional call
    static double get(const double lower_bound, const double upper_bound,
                      const std::optional<unsigned> seed = std::nullopt);

private:

    void reset() override;
    UniformRealDistribution* clone_impl() const override;
    JSON to_json_impl() const override;

    std::uniform_real_distribution<> _distr;

};

} // namespace NeuroEvo

#endif
