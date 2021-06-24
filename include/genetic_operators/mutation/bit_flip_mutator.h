#ifndef _BIT_FLIP_MUTATOR_H_
#define _BIT_FLIP_MUTATOR_H_

#include <genetic_operators/mutation/mutator.h>

namespace NeuroEvo {

class BitFlipMutator : public Mutator<bool>
{

public:

    BitFlipMutator(const double mutation_rate);

    void mutate(std::vector<bool>& genes) override;

private:

    void reset_mutator(const std::optional<unsigned>& seed) override {};

    BitFlipMutator* clone_impl() const override
    {
        return new BitFlipMutator(*this);
    }

};

} // namespace NeuroEvo

#endif
