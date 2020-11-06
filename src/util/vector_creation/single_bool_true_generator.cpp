#include <util/vector_creation/single_bool_true_generator.h>

namespace NeuroEvo {

SingleBoolTrueGenerator::SingleBoolTrueGenerator(const unsigned vector_size) :
    VectorCreationPolicy<bool>(vector_size) {}

std::vector<bool> SingleBoolTrueGenerator::generate_vector(const unsigned run_num)
{
    std::vector<bool> generated_vector(this->_vector_size, false);
    generated_vector.at(run_num) = true;
    return generated_vector;
}

} // namespace NeuroEvo
