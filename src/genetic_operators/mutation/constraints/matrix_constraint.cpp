#include <genetic_operators/mutation/constraints/matrix_constraint.h>

namespace NeuroEvo {
namespace Mutators {

MatrixConstraint::MatrixConstraint(const unsigned HEIGHT, const unsigned WIDTH) :
    _HEIGHT(HEIGHT),
    _WIDTH(WIDTH) {}

//Check that there is a 1 in all the matrix rows
bool MatrixConstraint::satisfied(std::vector<double>& vector) {

    for(unsigned i = 0; i < _HEIGHT; i++) {

        bool is_one = false;
        for(unsigned j = 0; j < _WIDTH; j++) {
            if(vector.at(i * _WIDTH + j) > 0.5) {
                is_one = true;
                break;
            }
        }

        if(!is_one)
            return false;

    }

    return true;

}

} // namespace Mutators
} // namespace NeuroEvo
