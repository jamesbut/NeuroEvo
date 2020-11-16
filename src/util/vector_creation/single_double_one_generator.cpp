#include <util/vector_creation/single_double_one_generator.h>

namespace NeuroEvo {

SingleDoubleOneGenerator::SingleDoubleOneGenerator(const unsigned vector_size) :
    VectorCreationPolicy<double>(vector_size) {}

std::vector<double> SingleDoubleOneGenerator::generate_vector(const unsigned run_num)
{
    std::vector<double> generated_vector(this->_vector_size, 0.);
    generated_vector.at(run_num) = 1.;
    return generated_vector;
}

} // namespace NeuroEvo
