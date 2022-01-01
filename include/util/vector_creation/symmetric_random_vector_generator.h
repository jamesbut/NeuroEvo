#ifndef _SYMMETRIC_RANDOM_VECTOR_GENERATOR_H_
#define _SYMMETRIC_RANDOM_VECTOR_GENERATOR_H_

/*
    This is a random vector generator but the vector produced is symmetric about the center
*/

#include <util/vector_creation/random_vector_generator.h>
#include <iostream>

namespace NeuroEvo {

template <typename T>
class SymmetricRandomVectorGenerator : public RandomVectorGenerator<T>
{

public:

    SymmetricRandomVectorGenerator(const unsigned vector_size,
                                   std::shared_ptr<Distribution<T>> vector_distr) :
        RandomVectorGenerator<T>(vector_size, vector_distr)
    {
        if(this->_vector_size % 2 != 0)
            throw std::domain_error("Cannot generate a symmetric vector of odd size!");
    }

    SymmetricRandomVectorGenerator(const JSON& json) :
        SymmetricRandomVectorGenerator(
            json.at({"vector_size"}),
            Factory<Distribution<T>>::create(json.at({"Distribution"}))
        ) {}

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

    JSON to_json_impl() const override
    {
        JSON json;
        json.emplace("name", "SymmetricRandomVectorGenerator");
        json.emplace("vector_size", this->_vector_size);
        json.emplace("Distribution", this->_vector_distr->to_json());
        return json;
    }

};

static Factory<VectorCreationPolicy<double>>::Registrar
    symmetric_random_vector_generator_registrar(
        "SymmetricRandomVectorGenerator",
        [](const JSON& json)
            {return std::make_shared<SymmetricRandomVectorGenerator<double>>(json);}
    );

} //namespace NeuroEvo

#endif
