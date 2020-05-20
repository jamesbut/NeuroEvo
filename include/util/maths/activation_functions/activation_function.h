#ifndef _ACTIVATION_FUNCTION_H_
#define _ACTIVATION_FUNCTION_H_

/*
    Defines a simple interface for an activation
    function.
*/

namespace NeuroEvo {

class ActivationFunction {

public:

    virtual ~ActivationFunction() = default;

    virtual double activate(const double x) = 0;

    auto clone() const 
    { 
        return std::unique_ptr<ActivationFunction>(clone_impl()); 
    };


protected:

    virtual ActivationFunction* clone_impl() const = 0;

};

} // namspace NeurEvo

#endif
