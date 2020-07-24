#include <util/statistics/distributions/char_distribution.h>

namespace NeuroEvo {

CharDistribution::CharDistribution(const std::set<char>& char_set, 
                                   const std::optional<std::vector<double>>& char_distr) :
    _char_set(char_set.begin(), char_set.end()) {

    //If a distribution over characters is not given then use a uniform distribution over
    //the character set
    if (char_distr)
        _distr = std::discrete_distribution<unsigned>(char_distr->begin(), char_distr->end());
    else {

        const double default_prob = 1. / (double)char_set.size();
        const std::vector<double> default_char_distr(char_set.size(), default_prob);
        _distr = std::discrete_distribution<unsigned>(default_char_distr.begin(), 
                                                      default_char_distr.end());

    }

}

char CharDistribution::next() {
    return _char_set[_distr(_rng)];
}

} // namepace NeuroEvo
