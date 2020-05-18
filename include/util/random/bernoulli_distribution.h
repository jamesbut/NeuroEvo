#ifndef _BERNOULLI_DISTRIBUTION_H_
#define _BERNOULLI_DISTRIBUTION_H_

#include <util/random/distribution.h>

/*
    A bernoulli distribution that returns a boolean according to a p parameter
*/

class BernoulliDistribution : public Distribution<bool> {

public:

    BernoulliDistribution(const double p = 0.5);

    bool next() override;

private:

   std::bernoulli_distribution _distr; 

};

#endif
