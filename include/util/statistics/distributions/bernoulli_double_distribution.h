#ifndef _BERNOULLI_DOUBLE_DISTRIBUTION_H_
#define _BERNOULLI_DOUBLE_DISTRIBUTION_H_

/*
    A bernoulli distribution in which the returned value is cast to a double
*/

#include <util/statistics/distributions/bernoulli_distribution.h>

namespace NeuroEvo {

class BernoulliDoubleDistribution : public Distribution<double> 
{

public: 

    BernoulliDoubleDistribution(const double p = 0.5);

    double next() override;
    void reset() override;

private: 

    BernoulliDoubleDistribution* clone_impl() const override
    {
        return new BernoulliDoubleDistribution(*this);
    }

    std::bernoulli_distribution _distr;

};

} // namespace NeuroEvo

#endif
