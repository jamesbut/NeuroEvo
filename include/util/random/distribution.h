#ifndef _DISTRIBUTION_H_
#define _DISTRIBUTION_H_

/*
    Abstract distribution class that defines a simple
    interface for a distribution.
    It is used - for example - when defining a distribution
    for the initial values of a genotype.
*/

#include <random>

namespace NeuroEvo {
namespace Utils {

class Distribution {

public:

    Distribution(const std::optional<unsigned> seed = std::optional<unsigned>()) {

        // If seed is provided seed with that value
        // Otherwise seed randomly
        if(seed) {

            _rng.seed(*seed);

        } else {

            std::random_device rand_dev;
            _rng.seed(rand_dev());

        }

    }

    virtual ~Distribution() = default;

    // Returns next value from the distribution
    virtual double next() = 0;

    // Sets random seed of distribution
    inline void set_seed(const unsigned seed) { _rng.seed(seed); }

protected:

    std::mt19937 _rng;

};

} // namespace Utils
} // namespace NeuroEvo

#endif
