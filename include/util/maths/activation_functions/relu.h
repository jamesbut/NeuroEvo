#ifndef _RELU_H_
#define _RELU_H_

namespace NeuroEvo {
namespace Utils {

class Relu : public ActivationFunction {

public:

    //TODO: Add a constructor with ReLU paramters

    double activate(const double x) override {

        return (x > 0) ? x : 0;

    }

protected:

    Relu* clone_impl() const override { return new Relu(*this); };

};

} // namespace Utils
} // namespace NeuroEvo

#endif
