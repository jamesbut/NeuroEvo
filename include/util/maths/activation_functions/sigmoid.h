#ifndef _SIGMOID_H_
#define _SIGMOID_H_

namespace NeuroEvo {
namespace Utils {

class Sigmoid : public ActivationFunction {

public:

    Sigmoid(const double K) :
        _K(K) {}

    double activate(const double x) override {

        return 1 / (1 + exp(-x / _K));

    }

protected:

    Sigmoid* clone_impl() const override { return new Sigmoid(*this); };

private:

    const double _K;

};

} // namespace Utils
} // namespace NeuroEvo

#endif
