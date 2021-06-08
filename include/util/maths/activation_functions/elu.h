#ifndef _ELU_H_
#define _ELU_H_

#include <util/maths/activation_functions/activation_function.h>

namespace NeuroEvo {

class ELU : public ActivationFunction
{

public:

    ELU(const double alpha = 1.);

    double activate(const double x) override;

private:

    ELU* clone_impl() const override;

    const double _alpha;

};

} // namespace NeuroEvo

#endif
