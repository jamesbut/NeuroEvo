#ifndef _ABSTRACT_SELECTION_H_
#define _ABSTRACT_SELECTION_H_

#include <organism.h>

class Selection {

public:

    // Selects an organism from a population.
    // A new organism is created as the child on which
    // modifications can be applied.
    virtual Organism select(std::vector<Organism>& orgs) = 0;

};

#endif
