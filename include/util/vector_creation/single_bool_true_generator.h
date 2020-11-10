#ifndef _SINGLE_BOOL_TRUE_GENERATOR_H_
#define _SINGLE_BOOL_TRUE_GENERATOR_H_

/*
    This generator generates a vector in which there is only a single true boolean
*/

#include <util/vector_creation/vector_creation_policy.h>

namespace NeuroEvo {

class SingleBoolTrueGenerator : public VectorCreationPolicy<bool>
{

public:

    SingleBoolTrueGenerator(const unsigned vector_size);

private:

    std::vector<bool> generate_vector(const unsigned run_num) override;
    void seeded() override {}
};

} // namespace NeuroEvo

#endif
