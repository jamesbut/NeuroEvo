#ifndef _RELU_H_
#define _RELU_H_

#include <util/maths/activation_functions/activation_function.h>

namespace NeuroEvo {

class ReLU : public ActivationFunction
{

public:

    ReLU();

    double activate(const double x) override;

private:

    JSON to_json() const override;
    ReLU* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
