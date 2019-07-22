#ifndef _CONSTRAINT_H_
#define _CONSTRAINT_H_

#include <vector>

class Constraint {

public:

    virtual bool satisfied(std::vector<double>& vector) = 0;

};

#endif
