#ifndef _CONCENTRIC_SQUARES_GENERATOR_H_
#define _CONCENTRIC_SQUARES_GENERATOR_H_

/*
    This generator creates a vector such that in a matrix of n x n, the image has concentric
    squares
*/

#include <util/vector_creation/vector_creation_policy.h>

namespace NeuroEvo {

class ConcentricSquaresGenerator : public VectorCreationPolicy<bool>
{

public:

    //The leading square is the value of the sqaure in the top left corner
    ConcentricSquaresGenerator(const unsigned image_width,
                               const unsigned leading_square);

private:

    std::vector<bool> generate_vector(const unsigned run_num) override;

    const unsigned _image_width;
    unsigned _leading_square;

};

} // namespace NeuroEvo

#endif
