#ifndef _BIT_FLIP_MUTATOR_H_
#define _BIT_FLIP_MUTATOR_H_

namespace NeuroEvo {

class BitFlipMutator : public Mutator<bool> {

public:

    BitFlipMutator(const double mutation_rate);

    bool mutate_gene(bool gene) override;

};

} // namespace NeuroEvo

#endif
