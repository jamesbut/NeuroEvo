#include <util/vector_creation/image_generators/concentric_squares_generator.h>
#include <util/maths/matrix.h>

namespace NeuroEvo {

ConcentricSquaresGenerator::ConcentricSquaresGenerator(const unsigned image_width,
                                                       const unsigned leading_square) : 
    VectorCreationPolicy<bool>(image_width * image_width),
    _image_width(image_width),
    _leading_square(leading_square) {}

std::vector<bool> ConcentricSquaresGenerator::generate_vector(const unsigned run_num) 
{

    //The algorithm to create the target image is somewhat complicated to index - I do 
    //apologise. Maybe there is an easier way to do it but this is what I figured out
    //for now
    Matrix<bool> target_image(_image_width, _image_width);
    target_image.fill(!_leading_square);

    //Upper half
    for(unsigned i = 0; i < (target_image.get_height() / 2 + 1); i++)
    {
        //Alternating part  
        for(unsigned j = 0; j < i; j++)
            if((j % 2) == 0)
                target_image.set(i, j, _leading_square);

        //Const part
        if((i % 2) == 0)
            for(unsigned j = 0; j < (target_image.get_width() - 2 * i); j++)
                target_image.set(i, j + i, _leading_square);

        //Alternating part
        for(unsigned j = (target_image.get_width() - i); j < target_image.get_width(); j++)
            if((j % 2) == static_cast<int>(target_image.get_width() % 2 == 0))
                target_image.set(i, j, _leading_square);

    }

    //Lower half
    const unsigned i_start = target_image.get_height() / 2 + 1;
    for(unsigned i = i_start; i < target_image.get_height(); i++)
    {
        //Alternating part  
        for(unsigned j = 0; j < (target_image.get_width() - i); j++)
            if((j % 2) == 0)
                target_image.set(i, j, _leading_square);

        //Const part
        if((i % 2) == static_cast<int>((target_image.get_width() % 2) == 0))
        {
            const unsigned num_consts = 2 * i - target_image.get_width() + 2;
            for(unsigned j = 0; j < num_consts; j++)
            {
                const unsigned j_index = target_image.get_width() - i - 1 + j;
                target_image.set(i, j_index, _leading_square);
            }
        }

        //Alternating part
        for(unsigned j = i; j < target_image.get_width(); j++)
            if((j % 2) == static_cast<int>(target_image.get_width() % 2 == 0))
                target_image.set(i, j, _leading_square);

    }

    return target_image.get_vector();

}

} // namespace NeuroEvo
