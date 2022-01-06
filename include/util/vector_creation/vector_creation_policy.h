#ifndef _VECTOR_CREATION_POLICY_H_
#define _VECTOR_CREATION_POLICY_H_

/*
    I was creating vectors in a number of different ways for both vector matching
    and image matching so I decided to abstract this out
*/

#include <vector>
#include <optional>
#include <data/json.h>

namespace NeuroEvo {

template <typename T>
class VectorCreationPolicy {

public:

    VectorCreationPolicy(const unsigned vector_size,
                         const std::optional<unsigned>& seed = std::nullopt) :
        _vector_size(vector_size),
        _seed(seed) {}

    virtual ~VectorCreationPolicy() = default;

    //The vector generated could be based upon the run number
    virtual std::vector<T> generate_vector(const unsigned run_num) = 0;

    unsigned get_vector_size() const
    {
        return _vector_size;
    }

    const std::optional<unsigned>& get_seed() const
    {
        return _seed;
    }

    void set_seed(const std::optional<unsigned>& seed)
    {
        _seed = seed;
        seeded();
    }

    JSON to_json() const
    {
        JSON json;
        json.emplace("vector_size", _vector_size);
        if(_seed.has_value())
            json.emplace("seed", _seed.value());
        //Add all json values of subclass
        json.emplace(to_json_impl());
        return json;
    }

    //Called when a new seed has been set
    virtual void seeded() = 0;

protected:

    virtual JSON to_json_impl() const = 0;

    const unsigned _vector_size;

    std::optional<unsigned> _seed;

};

} // namespace NeuroEvo

#endif
