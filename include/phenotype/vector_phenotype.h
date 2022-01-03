#ifndef _VECTOR_PHENOTYPE_H_
#define _VECTOR_PHENOTYPE_H_

/* A vector phenotype is a template class where the phenotype is represented as a vector
 * of generics. It is not a neural network, but this class is useful when doing general
 * optimisation.
 */

#include <phenotype/phenotype.h>

namespace NeuroEvo {

template <typename T>
class VectorPhenotype : public Phenotype<T>
{

public:

    VectorPhenotype(const std::vector<T>& traits) :
        Phenotype<T>(traits.size()),
        _traits(traits) {}

    std::vector<T> activate(const std::vector<double>& inputs) override
    {
        return _traits;
    }

    void reset() override {}

    void print(std::ostream& os) const override
    {
        os << "[";
        for(std::size_t i = 0; i < _traits.size(); i++)
        {
            os << _traits[i];
            if(i != _traits.size()-1)
                os << ",";
        }
        os << "]";
    }

private:

    JSON to_json_impl() const override
    {
        JSON json;
        json.emplace("name", "VectorPhenotype");
        json.emplace("traits", _traits);
        return json;
    }

    VectorPhenotype* clone_impl() const override
    {
        return new VectorPhenotype(*this);
    }

    std::vector<T> get_params() const override
    {
        return _traits;
    }

    const std::vector<T> _traits;

};

} // namespace NeuroEvo

#endif
