#ifndef _UNIFORM_UNSIGNED_DISTRIBUTION_H_
#define _UNIFORM_UNSIGNED_DISTRIBUTION_H_

/*
 * A uniform distribution over unsigned ints
 */

#include <util/statistics/distributions/distribution.h>

namespace NeuroEvo {

class UniformUnsignedDistribution : public Distribution<unsigned long>
{
   
public:

    UniformUnsignedDistribution(const unsigned long lower_bound,
                                const unsigned long upper_bound,
                                const std::optional<unsigned> seed = std::nullopt);

    unsigned long next() override;

    //Functional call to get a number between bounds
    //Default bound values are the full range able to be stored in unsigned longs
    static unsigned long get(const unsigned long lower_bound = 
                                 std::numeric_limits<unsigned long>::lowest(),
                             const unsigned long upper_bound = 
                                 std::numeric_limits<unsigned long>::max());

private:

    void reset() override;

    std::uniform_int_distribution<unsigned long> _distr;

};

} // namespace NeuroEvo

#endif 
