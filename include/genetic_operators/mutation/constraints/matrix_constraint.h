#ifndef _MATRIX_CONSTRAINT_H_
#define _MATRIX_CONSTRAINT_H_

/*
    This matrix constraint assumes that the vector of
    genes is a matrix and imposes the constaint that
    there must be a 1 in all rows.
*/

#include <genetic_operators/mutation/constraints/constraint.h>

class MatrixConstraint : public Constraint {

public:

    MatrixConstraint(const unsigned HEIGHT, const unsigned WIDTH);

    bool satisfied(std::vector<double>& vector) override;

private:

    const unsigned _HEIGHT;
    const unsigned _WIDTH;

};

#endif
