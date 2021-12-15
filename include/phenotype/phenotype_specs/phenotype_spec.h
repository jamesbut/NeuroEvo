#ifndef _PHENOTYPE_SPEC_H_
#define _PHENOTYPE_SPEC_H_

/*
    A PhenotypeSpec defines the specification of the phenotype
    to be used.
*/

#include <phenotype/phenotype.h>
#include <data/json.h>
#include <util/factory.h>

namespace NeuroEvo {

class PhenotypeSpec
{

public:

    PhenotypeSpec(const unsigned num_params, const bool trace = false);
    PhenotypeSpec(const JSON& json);

    virtual ~PhenotypeSpec() = default;

    unsigned get_num_params() const;
    bool get_trace() const;
    bool get_print_traits() const;

    void set_trace(const bool trace);

    JSON to_json() const
    {
        JSON json;
        json.emplace("num_params", _num_params);
        json.emplace("trace", _trace);
        json.emplace("print_traits", _print_traits);
        //Add all key value pairs from derived class
        for(auto& [key, value] : to_json_impl().items())
            json.emplace(key, value);
        return json;
    }

    auto clone() const
    {
        return std::unique_ptr<PhenotypeSpec>(clone_impl());
    }

protected:

    virtual JSON to_json_impl() const = 0;

    virtual PhenotypeSpec* clone_impl() const = 0;

    unsigned _num_params;

    bool _trace;
    bool _print_traits;

};

} // namespace NeuroEvo

#endif
