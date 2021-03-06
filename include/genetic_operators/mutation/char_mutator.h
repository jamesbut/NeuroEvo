#ifndef _CHAR_MUTATOR_H_
#define _CHAR_MUTATOR_H_

/*
 * A Char Mutator mutates characters according to a distribution over some set of characters
 */

#include <genetic_operators/mutation/mutator.h>
#include <set>
#include <util/statistics/distributions/char_distribution.h>

namespace NeuroEvo {

class CharMutator : public Mutator<char>
{

public:

    CharMutator(const double mutation_rate, const std::set<char>& char_set,
                const std::optional<std::vector<double>>& char_distr = std::nullopt);

    void mutate(std::vector<char>& genes) override;

private:

    CharDistribution _char_distr;

};

} // namespace NeuroEvo

#endif
