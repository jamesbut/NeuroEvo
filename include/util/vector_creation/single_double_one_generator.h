#ifndef _SINGLE_DOUBLE_ONE_GENERATOR_H_
#define _SINGLE_DOUBLE_ONE_GENERATOR_H_

/*
    This is the double equivalent of the SingleBoolTrueGenerator
*/

#include <util/vector_creation/vector_creation_policy.h>

namespace NeuroEvo {

class SingleDoubleOneGenerator : public VectorCreationPolicy<double>
{

public:

    SingleDoubleOneGenerator(const unsigned vector_size);

private:

    std::vector<double> generate_vector(const unsigned run_num) override;
    void seeded() override {}

};

} // namespace NeuroEvo

#endif
