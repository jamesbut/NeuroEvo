#include <phenotype/neural_network/hebbs_layer.h>

namespace NeuroEvo {

HebbsLayer::HebbsLayer(const LayerSpec& layer_spec, const bool trace) :
    Layer(layer_spec, trace)
{
    for(unsigned i = 0; i < layer_spec.get_num_neurons(); i++)
        _neurons.push_back(
            std::unique_ptr<Neuron>(new HebbsNeuron(layer_spec, trace))
        );
}

void HebbsLayer::set_learning_rates(const std::vector<double>& learning_rates) 
{

    auto start = learning_rates.begin();
    auto end = learning_rates.begin();

    //Dynamically cast to HebbsNeuron in order to call Hebbs related methods
    std::vector<HebbsNeuron*> hebbs_neurons;
    hebbs_neurons.reserve(_neurons.size());
    for(auto& neuron : _neurons)
        hebbs_neurons.push_back(dynamic_cast<HebbsNeuron*>(neuron.get()));

    for(auto& neuron : hebbs_neurons)
    {
        end += _params_per_neuron;

        std::vector<double> tempW(start, end);
        neuron->set_learning_rates(tempW);

        start += _params_per_neuron;
     }

}

} // namespace NeuroEvo
