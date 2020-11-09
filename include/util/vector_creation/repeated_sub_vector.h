#ifndef _REPEATED_SUB_VECTOR_H_
#define _REPEATED_SUB_VECTOR_H_

/*
    This generator creates a random sub vector and repeats it through the vector.
*/

#include <util/vector_creation/random_vector_generator.h>
#include <iostream>

namespace NeuroEvo {

template <typename T>
class RepeatedSubVector : public RandomVectorGenerator<T> {

public:

    RepeatedSubVector(const unsigned sub_vector_size,
                      const unsigned vector_size,
                      std::shared_ptr<Distribution<T>> vector_distr) :
        RandomVectorGenerator<T>(vector_size, vector_distr),
        _sub_vector_size(sub_vector_size) 
    {
        if(vector_size % sub_vector_size != 0)
        {
            std::cerr << "sub_vector_size must be a factor of vector_size!" << std::endl;
            exit(0);
        }
    }

private:

    std::vector<T> generate_vector(const unsigned run_num) override
    {
        std::vector<T> sub_vector(_sub_vector_size);
        for(unsigned i = 0; i < sub_vector.size(); i++)
        {
            sub_vector[i] = this->_vector_distr->next();
        }

        std::vector<T> final_vector(0); 
        const unsigned num_repeated_sub_vectors = this->_vector_size / _sub_vector_size;
        for(unsigned i = 0; i < num_repeated_sub_vectors; i++)
        {
            final_vector.insert(final_vector.end(), sub_vector.begin(), sub_vector.end()); 
        }

        return final_vector;

    }

    const unsigned _sub_vector_size;

};

} // namespace NeuroEvo

#endif
