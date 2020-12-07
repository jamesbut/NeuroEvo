#include <util/vector_creation/square_symmetric_vector.h>
#include <iostream>

namespace NeuroEvo {

SquareSymmetricVector::SquareSymmetricVector(
    const unsigned vector_size,
    std::shared_ptr<Distribution<double>> vector_distr) :
    RandomVectorGenerator<double>(vector_size, vector_distr) 
    {
        if(this->_vector_size % 2 != 0)
        {
            std::cerr << "Cannot generate a symmetric vector of odd size!" << std::endl;
            exit(0);
        }
    }

std::vector<double> SquareSymmetricVector::generate_vector(const unsigned run_num)
{
    std::vector<double> generated_vector;
    generated_vector.reserve(this->_vector_size);

    //Randomly generate vector of half the size, duplicate, square, reverse and concatenate
    const unsigned num_unique_elements = this->_vector_size/2;
    std::vector<double> first_half_vector;
    first_half_vector.reserve(num_unique_elements);

    for(unsigned i = 0; i < num_unique_elements; i++)   
        first_half_vector.push_back(this->_vector_distr->next());

    //Create symmetric squared second half
    std::vector<double> second_half_vector(first_half_vector);
    std::reverse(second_half_vector.begin(), 
                 second_half_vector.end());
    std::transform(second_half_vector.begin(), second_half_vector.end(), 
                   second_half_vector.begin(),
                   [](const auto v) {return std::pow(v, 2);});

    //Concatenate
    first_half_vector.insert(first_half_vector.end(),
                             second_half_vector.begin(),
                             second_half_vector.end());
    generated_vector = first_half_vector;

    return generated_vector;
}

} // namespace NeuroEvo
