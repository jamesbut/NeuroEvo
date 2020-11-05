#ifndef _VECTOR_CREATION_POLICIES_H_
#define _VECTOR_CREATION_POLICIES_H_

/*
    I was creating vectors in a number of different ways for both vector matching
    and image matching so I decided to abstract this out 
*/

#include <vector>
#include <util/statistics/distributions/distribution.h>
#include <iostream>

namespace NeuroEvo {

template <typename T>
class VectorCreationPolicy {

public: 

    VectorCreationPolicy(const unsigned vector_size) :
        _vector_size(vector_size) {}

    virtual ~VectorCreationPolicy() = default;
    
    //The vector generated could be based upon the run number
    virtual std::vector<T> generate_vector(const unsigned run_num) = 0;

protected: 

    const unsigned _vector_size;

};

//This generator generates according to some probability distribution over T
template <typename T>
class RandomGenerator : public VectorCreationPolicy<T> {

public: 

    RandomGenerator(const unsigned vector_size, 
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

};

//This is a random generator but the vector produced is symmetric about the center
template <typename T>
class SymmetricRandomGenerator : public RandomGenerator<T>
{

public:

    SymmetricRandomGenerator(const unsigned vector_size,
                             std::shared_ptr<Distribution<T>> vector_distr) :
        RandomGenerator<T>(vector_size, vector_distr) 
    {
        if(this->_vector_size % 2 != 0)
        {
            std::cerr << "Cannot generate a symmetric vector of odd size!" << std::endl;
            exit(0);
        }
    }

private:

    std::vector<T> generate_vector(const unsigned run_num) override
    {
        std::vector<T> matching_vector;
        matching_vector.reserve(this->_vector_size);

        //Randomly generate vector of half the size, duplicate, reverse and concatenate
        const unsigned num_unique_elements = this->_vector_size/2;
        std::vector<T> first_half_matching_vector;
        first_half_matching_vector.reserve(num_unique_elements);

        for(unsigned i = 0; i < num_unique_elements; i++)   
            first_half_matching_vector.push_back(this->_vector_distr->next());

        //Create symmetric second half
        std::vector<T> second_half_matching_vector(first_half_matching_vector);
        std::reverse(second_half_matching_vector.begin(), 
                     second_half_matching_vector.end());

        //Concatenate
        first_half_matching_vector.insert(first_half_matching_vector.begin(),
                                          second_half_matching_vector.begin(),
                                          second_half_matching_vector.end());
        matching_vector = first_half_matching_vector;

        return matching_vector;

    }

};

//This generator generates a vector in which there is only a single true boolean
class SingleBoolTrueGenerator : public VectorCreationPolicy<bool>
{

public:

    SingleBoolTrueGenerator(const unsigned vector_size) :
        VectorCreationPolicy<bool>(vector_size) {}

private:

    std::vector<bool> generate_vector(const unsigned run_num) override
    {
        std::vector<bool> generated_vector(this->_vector_size, false);
        generated_vector.at(run_num) = true;
        return generated_vector;
    }
};

} // namespace NeuroEvo

#endif
