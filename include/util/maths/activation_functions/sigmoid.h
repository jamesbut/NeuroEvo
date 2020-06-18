#ifndef _SIGMOID_H_
#define _SIGMOID_H_

#include <util/maths/activation_functions/activation_function.h>
#include <math.h>

namespace NeuroEvo {

class Sigmoid : public ActivationFunction 
{

public:

    Sigmoid(const double k = 1.) :
        _k(k) {}

    double activate(const double x) override 
    {
        return 1 / (1 + exp(-x / _k));
    }

protected:

    Sigmoid* clone_impl() const override { return new Sigmoid(*this); };

private:

    const double _k;

};

} // namespace NeuroEvo

#endif
