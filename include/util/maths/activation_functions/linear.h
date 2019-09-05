#ifndef _LINEAR_H_
#define _LINEAR_H_

namespace NeuroEvo {
namespace Utils {

class Linear : public ActivationFunction {

public:

    double activate(const double x) override {

        return x;

    }

protected:

    Linear* clone_impl() const override { return new Linear(*this); };

};

} // namespace Utils
} // namespace NeuroEvo

#endif
