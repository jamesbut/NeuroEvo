#ifndef _LAYER_H_
#define _LAYER_H_

/*
    A layer consists of a vector of neurons.
*/

#include <phenotype/neural_network/neuron.h>
#include <vector>

namespace NeuroEvo {

class Layer {

public:

    Layer(const LayerSpec& layer_spec, const bool trace);

    Layer(const Layer& layer);
    Layer& operator=(const Layer& layer) = default;
    ~Layer() = default;
    Layer(Layer&& layer) = default;
    Layer& operator=(Layer&& layer) = default;

    void set_weights(std::vector<double>& weights);
    unsigned get_number_of_params() const;
    std::vector<double> evaluate(std::vector<double>& inputs);

    void reset();

    void print_params() const;

private:

    //TODO: Have all the specs as references
    LayerSpec _layer_spec;
    bool _trace;

    std::vector<std::unique_ptr<Neuron> > _neurons;

    void print_outputs(std::vector<double>& outputs);

};

} // namespace NeuroEvo

#endif
