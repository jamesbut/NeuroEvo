#ifndef _LINEAR_H_
#define _LINEAR_H_

#include <util/maths/activation_functions/activation_function.h>

namespace NeuroEvo {

class Linear : public ActivationFunction
{

public:

    Linear();

    double activate(const double x) override;

private:

    Linear* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
