#ifndef _RELU_H_
#define _RELU_H_

#include <util/maths/activation_functions/activation_function.h>

namespace NeuroEvo {

class ReLU : public ActivationFunction 
{

public:

    double activate(const double x) override 
    {
        return (x > 0) ? x : 0;
    }

protected:

    ReLU* clone_impl() const override 
    { 
        return new ReLU(*this); 
    };

};

} // namespace NeuroEvo

#endif
