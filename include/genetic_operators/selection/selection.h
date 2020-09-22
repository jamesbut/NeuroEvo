#ifndef _ABSTRACT_SELECTION_H_
#define _ABSTRACT_SELECTION_H_

#include <organism.h>

namespace NeuroEvo {

template <typename G, typename T>
class Selection {

public:

    virtual ~Selection() = default;

    // Selects an organism from a population.
    // A new organism is created as the child on which
    // modifications can be applied.
    virtual Organism<G, T> select(const std::vector<Organism<G, T>>& orgs) = 0;

    auto clone() const 
    {
        return std::unique_ptr<Selection>(clone_impl());
    }

protected:

    virtual Selection* clone_impl() const = 0;

};

} // namespace NeuroEvo

#endif
