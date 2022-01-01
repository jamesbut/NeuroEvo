#ifndef _CHAR_DISTRIBUTION_H_
#define _CHAR_DISTRIBUTION_H_

#include <util/statistics/distributions/distribution.h>
#include <set>

/*
    A char distribution is a distribution that draws from a set
    of unique characters. It does this using a discrete distribution
    in which you can set the relative probability of drawing each
    character from the set.
*/

namespace NeuroEvo {

class CharDistribution : public Distribution<char>
{

public:

    CharDistribution(const std::set<char>& char_set,
                     const std::optional<std::vector<double>>& char_distr = std::nullopt);

    char next() override;

private:

    void reset() override;
    CharDistribution* clone_impl() const override;
    JSON to_json_impl() const override;

    //Better to store this as a vector because we can index in O(1) time as
    //oppose to O(n)
    const std::vector<char> _char_set;
    std::discrete_distribution<unsigned> _distr;

};

} // namespace NeuroEvo

#endif
