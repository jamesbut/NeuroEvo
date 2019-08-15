#ifndef _NEURON_H_
#define _NEURON_H_

/*
    A neuron holds the connection weights of the connections
    that enter it.
*/

#include <phenotype/phenotype_specs/layer_spec.h>
#include <vector>

namespace NeuroEvo {
namespace Phenotypes {

class Neuron {

public:

    Neuron(const LayerSpec& LAYER_SPEC, const bool TRACE);

    virtual void set_weights(std::vector<double>& weights);
    virtual double evaluate(std::vector<double>& inputs);

    virtual void reset();
    auto clone() const { return std::unique_ptr<Neuron>(clone_impl()); };

    void print_weights();

protected:

    virtual Neuron* clone_impl() const { return new Neuron(*this); };

    const LayerSpec _LAYER_SPEC;

    const bool _TRACE;

    double _previous_output;
    std::vector<double> _weights;


};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
