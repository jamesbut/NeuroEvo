#ifndef _RANDOM_VECTOR_GENERATOR_H_
#define _RANDOM_VECTOR_GENERATOR_H_

/*
    This generator generates according to some probability distribution over T
*/

#include <util/vector_creation/vector_creation_policy.h>
#include <util/statistics/distributions/distribution.h>

namespace NeuroEvo {

template <typename T>
class RandomVectorGenerator : public VectorCreationPolicy<T> {

public: 

    RandomVectorGenerator(const unsigned vector_size, 
                          std::shared_ptr<Distribution<T>> vector_distr) :
        VectorCreationPolicy<T>(vector_size),
        _vector_distr(vector_distr) {}

protected:

    std::vector<T> generate_vector(const unsigned run_num) override
    {
        std::vector<T> generated_vector(this->_vector_size);
        for(unsigned i = 0; i < generated_vector.size(); i++)
            generated_vector[i] = _vector_distr->next();
        return generated_vector;
    }

    std::shared_ptr<Distribution<T>> _vector_distr;

private:

    void seeded() override
    {
        _vector_distr->set_seed(this->_seed.value());
    }


};

} // namespace NeuroEvo

#endif
