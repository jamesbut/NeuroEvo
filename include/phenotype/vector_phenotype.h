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
        _traits(traits) {}

    std::vector<T> activate(const std::vector<double>& inputs) override 
    {
        return _traits;
    }

    void reset() override {}

    void print_params() override 
    {
        for(const auto& trait : _traits)
            std::cout << trait << std::endl;
    }

protected:

    VectorPhenotype* clone_impl() const override
    {
        return new VectorPhenotype(*this);
    }

private:

    const std::vector<T> _traits;

};

} // namespace NeuroEvo

#endif
