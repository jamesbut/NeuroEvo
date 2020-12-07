#ifndef _SQUARE_SYMMETRIC_VECTOR_H_
#define _SQUARE_SYMMETRIC_VECTOR_H_

/*
    This random vector generator generates a vector of doubles that has the property that
    the opposite values in the vector have a squared relationship between them. 
*/

#include <util/vector_creation/random_vector_generator.h>

namespace NeuroEvo {

class SquareSymmetricVector : public RandomVectorGenerator<double>
{

public:

    SquareSymmetricVector(const unsigned vector_size,
                          std::shared_ptr<Distribution<double>> vector_distr);

private:

    std::vector<double> generate_vector(const unsigned run_num) override;

};

}

#endif
