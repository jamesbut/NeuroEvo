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

    void set_weights(const std::vector<double>& weights);
    unsigned get_number_of_weights() const;
    std::vector<double> evaluate(const std::vector<double>& inputs);

    void reset();

    void print_weights() const;
    void print_params() const;
    void print_weights_to_file(std::ofstream& file) const;
    void print_outputs_to_file(std::ofstream& file) const;

protected:

    //TODO: Have all the specs as references
    LayerSpec _layer_spec;
    bool _trace;

    std::vector<std::unique_ptr<Neuron> > _neurons;

    void print_outputs(const std::vector<double>& outputs);

};

} // namespace NeuroEvo

#endif
