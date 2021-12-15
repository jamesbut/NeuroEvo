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

    Phenotype(const bool trace) :
        _trace(trace) {}

    Phenotype(const unsigned num_params, const bool trace) :
        _num_params(num_params),
        _trace(trace) {}

    //Putting virtual here seems to make a big difference to the memory leak
    virtual ~Phenotype() = default;

    virtual std::vector<T> activate(
        const std::vector<double>& inputs = std::vector<double>()) = 0;
    virtual void reset() = 0;

    virtual JSON to_json() const
    {
        JSON json;
        json.emplace("num_params", _num_params.value());
        json.emplace("trace", _trace);
        for(auto& [key, value] : to_json_impl().items())
            json.emplace(key, value);
        return json;
    }

    auto clone_phenotype() const { return std::unique_ptr<Phenotype>(clone_impl()); }

    const std::optional<unsigned>& get_num_params() const
    {
        return _num_params;
    }

    virtual void set_trace(const bool trace)
    {
        _trace = trace;
    }

    friend std::ostream& operator<<(std::ostream& os, const Phenotype& phenotype)
    {
        phenotype.print(os);
        return os;
    }

protected:

    virtual JSON to_json_impl() const = 0;
    virtual Phenotype* clone_impl() const = 0;

    std::optional<unsigned> _num_params;

    bool _trace;

private:

    virtual void print(std::ostream& os) const  = 0;

};

} // namespace NeuroEvo

#endif
