#ifndef _ABSTRACT_SELECTION_H_
#define _ABSTRACT_SELECTION_H_

#include <organism.h>

namespace NeuroEvo {

template <typename G>
class Selection {

public:

    virtual ~Selection() = default;

    // Selects an organism from a population.
    // A new organism is created as the child on which
    // modifications can be applied.
    virtual Organism<G> select(std::vector<Organism<G>>& orgs) = 0;

};

} // namespace NeuroEvo

#endif
