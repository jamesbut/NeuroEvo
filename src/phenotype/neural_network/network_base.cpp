#include <phenotype/neural_network/network_base.h>

namespace NeuroEvo {

NetworkBase::NetworkBase(const bool trace) :
    Phenotype(trace) {}

std::shared_ptr<ActivationFunction> NetworkBase::get_final_layer_activ_func() const
{
    return _final_layer_activ_func;
}

unsigned NetworkBase::get_num_inputs() const
{
    return _num_inputs;
}

unsigned NetworkBase::get_num_outputs() const
{
    return _num_outputs;
}

} // namespace NeuroEvo
