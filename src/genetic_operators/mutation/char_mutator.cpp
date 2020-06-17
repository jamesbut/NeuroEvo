#include <genetic_operators/mutation/char_mutator.h>

namespace NeuroEvo {

CharMutator::CharMutator(const double mutation_rate, const std::set<char>& char_set,
                         const std::optional<std::vector<double>>& char_distr) :
    Mutator(mutation_rate),
    _char_distr(char_set, char_distr) {}

char CharMutator::mutate_gene(char gene) {

    char new_gene = gene;
    while(new_gene == gene)
        new_gene = _char_distr.next(); 

    return new_gene;

}

} // namespace NeuroEvo
