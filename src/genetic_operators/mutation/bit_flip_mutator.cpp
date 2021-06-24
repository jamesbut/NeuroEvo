#include <genetic_operators/mutation/bit_flip_mutator.h>

namespace NeuroEvo {

BitFlipMutator::BitFlipMutator(const double mutation_rate) :
    Mutator(mutation_rate) {}

void BitFlipMutator::mutate(std::vector<bool>& genes) {

    for(std::size_t i = 0; i < genes.size(); i++)
        if(should_mutate())
            genes[i] = !genes[i];

}

} // namespace NeuroEvo
