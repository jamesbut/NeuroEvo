#ifndef _BERNOULLI_DISTRIBUTION_H_
#define _BERNOULLI_DISTRIBUTION_H_

#include <util/statistics/distributions/distribution.h>

/*
    A bernoulli distribution that returns a boolean according to a p parameter
*/

namespace NeuroEvo {

class BernoulliDistribution : public Distribution<bool> {

public:

    BernoulliDistribution(const double p = 0.5);

    bool next() override;
    void reset() override;

private:

    BernoulliDistribution* clone_impl() const override
    {
        return new BernoulliDistribution(*this);
    }

   std::bernoulli_distribution _distr; 

};

} // namespace NeuroEvo

#endif
