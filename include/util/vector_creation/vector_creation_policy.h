#ifndef _VECTOR_CREATION_POLICY_H_
#define _VECTOR_CREATION_POLICY_H_

/*
    I was creating vectors in a number of different ways for both vector matching
    and image matching so I decided to abstract this out 
*/

#include <vector>

namespace NeuroEvo {

template <typename T>
class VectorCreationPolicy {

public: 

    VectorCreationPolicy(const unsigned vector_size) :
        _vector_size(vector_size) {}

    virtual ~VectorCreationPolicy() = default;
    
    //The vector generated could be based upon the run number
    virtual std::vector<T> generate_vector(const unsigned run_num) = 0;

    unsigned get_vector_size() const
    {
        return _vector_size;
    }

protected: 

    const unsigned _vector_size;

};

} // namespace NeuroEvo

#endif
