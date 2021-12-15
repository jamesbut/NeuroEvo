#ifndef _ACTIVATION_FUNCTION_H_
#define _ACTIVATION_FUNCTION_H_

/*
    Defines a simple interface for an activation
    function.
*/

#include <memory>
#include <util/maths/number_bound.h>
#include <data/json.h>

namespace NeuroEvo {

class ActivationFunction
{

public:

    ActivationFunction(const NumberBound& lower_bound, const NumberBound& upper_bound);

    virtual ~ActivationFunction() = default;

    virtual double activate(const double x) = 0;

    JSON to_json() const;
    auto clone() const
    {
        return std::unique_ptr<ActivationFunction>(clone_impl());
    }

    const NumberBound& get_lower_bound() const;
    const NumberBound& get_upper_bound() const;

private:

    virtual JSON to_json_impl() const = 0;
    virtual ActivationFunction* clone_impl() const = 0;

    const NumberBound _lower_bound;
    const NumberBound _upper_bound;

};

} // namspace NeurEvo

#endif
