#ifndef _PHENOTYPE_H_
#define _PHENOTYPE_H_

/*
    Specifies the typical Phenotype.
    All phenotypes implement an activate function
    that takes and returns a vector of doubles.
*/

#include <vector>
#include <genotype/genotype.h>

#include <iostream>

namespace NeuroEvo {

template <typename T>
class Phenotype 
{

public:

    Phenotype() = default;

    Phenotype(const unsigned num_params) :
        _num_params(num_params) {}

    //Putting virtual here seems to make a big difference to the memory leak
    virtual ~Phenotype() = default;

    virtual std::vector<T> activate(const std::vector<double>& inputs) = 0;
    virtual void reset() = 0;

    auto clone() const { return std::unique_ptr<Phenotype>(clone_impl()); }

    virtual void print_params() = 0;

    const std::optional<unsigned>& get_num_params() const
    {
        return _num_params;
    }

protected:

    virtual Phenotype* clone_impl() const = 0;

    std::optional<unsigned> _num_params;

};

} // namespace NeuroEvo

#endif
