#ifndef _HEBBS_LAYER_H_
#define _HEBBS_LAYER_H_

#include <phenotype/neural_network/hebbs_neuron.h>
#include <phenotype/neural_network/layer.h>
#include <vector>

namespace NeuroEvo {

class HebbsLayer : Layer {

public:

    HebbsLayer(const LayerSpec& layer_spec, const bool trace);

    void set_learning_rates(const std::vector<double>& learning_params);

};

} // namespace NeuroEvo

#endif
