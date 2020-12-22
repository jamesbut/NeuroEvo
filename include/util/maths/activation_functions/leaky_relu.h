#ifndef _LEAKY_RELU_H_
#define _LEAKY_RELU_H_

#include <util/maths/activation_functions/activation_function.h>

namespace NeuroEvo {

class LeakyReLU : public ActivationFunction
{

public:

    LeakyReLU(const double negative_slope = 0.01) :
        ActivationFunction(),
        _negative_slope(negative_slope) {}

    double activate(const double x) override
    {
        return (x > 0) ? x : _negative_slope * x;
    }

private:

    LeakyReLU* clone_impl() const override
    {
        return new LeakyReLU(*this);
    }

    const double _negative_slope;

};

} // namespace NeuroEvo

#endif
