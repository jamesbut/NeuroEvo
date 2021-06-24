#include <genetic_operators/mutation/char_mutator.h>

namespace NeuroEvo {

CharMutator::CharMutator(const double mutation_rate, const std::set<char>& char_set,
                         const std::optional<std::vector<double>>& char_distr) :
    Mutator(mutation_rate),
    _char_distr(char_set, char_distr) {}

void CharMutator::mutate(std::vector<char>& genes)
{

    for(auto& gene : genes)
    {
        if(!should_mutate())
            continue;

        char new_gene = gene;
        while(new_gene == gene)
            new_gene = _char_distr.next();

    }

}

} // namespace NeuroEvo
