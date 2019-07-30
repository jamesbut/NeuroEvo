#ifndef _CONSTRAINT_H_
#define _CONSTRAINT_H_

#include <vector>

namespace NeuroEvo {
namespace Mutators {

class Constraint {

public:

    virtual bool satisfied(std::vector<double>& vector) = 0;

};

} // namespace Mutators
} // namespace NeuroEvo

#endif
