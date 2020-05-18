#ifndef _DISTRIBUTION_H_
#define _DISTRIBUTION_H_

/*
    Abstract templated distribution class that defines a simple
    interface for a distribution.
    It allows for the setting and modifcation of seeds and returns
    a value of type T according to some distribution in the sub class.
*/

#include <random>
#include <optional>

namespace NeuroEvo {
namespace Utils {

template <typename T>
class Distribution {

public:

    Distribution(const std::optional<unsigned> seed = std::nullopt) {

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
    virtual T next() = 0;

    // Sets random seed of distribution
    inline void set_seed(const unsigned seed) { _rng.seed(seed); }

protected:

    std::mt19937 _rng;

};

} // namespace Utils
} // namespace NeuroEvo

#endif
