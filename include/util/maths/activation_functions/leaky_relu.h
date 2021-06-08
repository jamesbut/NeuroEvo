#ifndef _LEAKY_RELU_H_
#define _LEAKY_RELU_H_

#include <util/maths/activation_functions/activation_function.h>

namespace NeuroEvo {

class LeakyReLU : public ActivationFunction
{

public:

    LeakyReLU(const double negative_slope = 0.01);

    double activate(const double x) override;

private:

    LeakyReLU* clone_impl() const override;

    const double _negative_slope;

};

} // namespace NeuroEvo

#endif
