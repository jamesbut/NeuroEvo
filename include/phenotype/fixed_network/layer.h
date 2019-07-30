#ifndef _LAYER_H_
#define _LAYER_H_

/*
    A layer consists of a vector of neurons.
*/

#include <phenotype/fixed_network/neuron.h>
#include <vector>

namespace NeuroEvo {
namespace Phenotypes {

class Layer {

public:

    Layer(const LayerSpec& layer_spec, const bool TRACE);

    Layer(const Layer& layer);
    Layer& operator=(const Layer& layer) = default;
    ~Layer() = default;
    Layer(Layer&& layer) = default;
    Layer& operator=(Layer&& layer) = default;

    void set_weights(std::vector<double>& weights);
    unsigned get_number_of_params();
    std::vector<double> evaluate(std::vector<double>& inputs);

    void reset();

    void print_params() {
        for(const auto& neuron : neurons)
            neuron->print_weights();
    };

private:

    //TODO: Have all the specs as references
    const LayerSpec _LAYER_SPEC;
    const bool _TRACE;

    std::vector<std::unique_ptr<Neuron> > neurons;

    void print_outputs(std::vector<double>& outputs);

};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
