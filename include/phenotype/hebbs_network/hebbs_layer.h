#ifndef _HEBBS_LAYER_H_
#define _HEBBS_LAYER_H_

#include <phenotype/hebbs_network/hebbs_neuron.h>
#include <vector>

class HebbsLayer {

public:

    HebbsLayer(const LayerSpec& layer_spec, const bool TRACE);

    void set_learning_rates(std::vector<double>& learning_params);
    void set_weights(std::vector<double>& weights);

    int get_number_of_connections();
    std::vector<double> evaluate(std::vector<double>& inputs);

    void print_weights();
    void print_weights_to_file(std::ofstream& file);
    void print_outputs_to_file(std::ofstream& file);

    void reset();

    void print_params() {
        for(auto& neuron : neurons)
            neuron.print_weights();
    }

private:

    const LayerSpec _LAYER_SPEC;
    const bool _TRACE;

    std::vector<HebbsNeuron> neurons;

    void print_outputs(std::vector<double>& outputs);

};

#endif
