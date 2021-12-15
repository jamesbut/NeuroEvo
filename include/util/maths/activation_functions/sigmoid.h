#ifndef _SIGMOID_H_
#define _SIGMOID_H_

#include <util/maths/activation_functions/activation_function.h>
#include <math.h>

namespace NeuroEvo {

class Sigmoid : public ActivationFunction
{

public:

    Sigmoid(const double k = 1.);

    double activate(const double x) override;

private:

    JSON to_json_impl() const override;
    Sigmoid* clone_impl() const override;

    const double _k;

};

} // namespace NeuroEvo

#endif
