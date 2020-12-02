#ifndef _ELU_H_
#define _ELU_H_

#include <util/maths/activation_functions/activation_function.h>

namespace NeuroEvo {

class ELU : public ActivationFunction
{

public: 

    ELU(const double alpha = 1.) :
        _alpha(alpha) {}

    double activate(const double x) override
    {
        return (x > 0) ? x : _alpha * (exp(x) - 1);
    }

private:

    ELU* clone_impl() const override
    {
        return new ELU(*this);
    }

    const double _alpha;

};

} // namespace NeuroEvo

#endif
