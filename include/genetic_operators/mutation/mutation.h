#ifndef _ABSTRACT_MUTATION_H_
#define _ABSTRACT_MUTATION_H_

#include <genetic_operators/mutation/constraints/constraint.h>
#include <memory>

namespace NeuroEvo {
namespace Mutators {

class Mutation {

public:

    // One can define a mutation contraint such that if a mutation
    // violates some condition, the mutation is not applied
    Mutation(Constraint* constraint) :
        _constraint(constraint) {}

    virtual ~Mutation() = default;

    virtual void mutate(std::vector<double>& vector) = 0;

protected:

    std::unique_ptr<Constraint> _constraint;

};

} // namespace Mutators
} // namespace NeuroEvo

#endif
