#include <genetic_operators/mutation/bit_flip_mutator.h>

namespace NeuroEvo {

BitFlipMutator::BitFlipMutator(const double mutation_rate) :
    Mutator(mutation_rate) {}

bool BitFlipMutator::mutate_gene(bool gene) {
    return !gene;
}

} // namespace NeuroEvo
