#include <phenotype/neural_network/hebbs_layer.h>

namespace NeuroEvo {

HebbsLayer::HebbsLayer(const LayerSpec& layer_spec, const bool trace) :
    Layer(layer_spec, trace) {}

void HebbsLayer::create_layer()
{
    for(unsigned i = 0; i < _num_neurons; i++)
        _neurons.push_back(
            std::unique_ptr<Neuron>(new HebbsNeuron(_inputs_per_neuron,
                                                    _neuron_type,
                                                    _activation_function,
                                                    _trace))
        );
}

void HebbsLayer::set_learning_rates(const std::vector<double>& learning_rates) 
{
    auto start = learning_rates.begin();
    auto end = learning_rates.begin();

    for(auto& neuron : _neurons)
    {
        end += _params_per_neuron;

        std::vector<double> tempW(start, end);
        neuron->set_learning_rates(tempW);

        start += _params_per_neuron;
     }

}

} // namespace NeuroEvo
