#ifndef _CHAR_DISTRIBUTION_H_
#define _CHAR_DISTRIBUTION_H_

#include "distribution.h"
#include <set>

/*
    A char distribution is a distribution that draws from a set
    of unique characters. It does this using a discrete distribution
    in which you can set the relative probability of drawing each
    character from the set.
*/

class CharDistribution : public Distribution<char> {

public:

    CharDistribution(const std::set<char>& char_set, 
                     const std::optional<std::vector<double>>& char_distr = std::nullopt);

    char next() override;

private:

    //Better to store this as a vector because we can index in O(1) time as oppose to O(n)
    const std::vector<char> _char_set;
    std::discrete_distribution<unsigned> _distr;

};

#endif
