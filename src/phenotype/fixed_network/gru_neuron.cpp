#include <phenotype/fixed_network/gru_neuron.h>

namespace NeuroEvo {

GRUNeuron::GRUNeuron(const LayerSpec& layer_spec, const bool trace) :
    Neuron(layer_spec, trace) 
{

    //Set weight indices
    _U_index = 0;
    _U_r_index = _U_index + _layer_spec._inputs_per_neuron;
    _U_u_index = _U_r_index + _layer_spec._inputs_per_neuron;

    _w_index = _U_u_index + _layer_spec._inputs_per_neuron;
    _w_r_index = _w_index + 1;
    _w_u_index = _w_r_index + 1;
    _b_index = _w_u_index + 1;
    _b_r_index = _b_index + 1;
    _b_u_index = _b_r_index + 1;

    //Initialise previous output
    _previous_output = 0.5;

}

void GRUNeuron::set_weights(std::vector<double>& weights) 
{
    _weights = weights;
}


double GRUNeuron::evaluate(std::vector<double>& inputs) 
{

    std::vector<double>::iterator weight_iter = _weights.begin();
    unsigned index = 0;

    //Reset gate
    double reset_input_sum = 0.0;

    for(unsigned i = 0; i < _layer_spec._inputs_per_neuron; i++) 
    {
        reset_input_sum += inputs.at(i) * _weights.at(_U_r_index + i);
        if(_trace) std::cout << inputs.at(i) << " x " << _weights.at(_U_r_index + i) << std::endl;
    }

    reset_input_sum += _previous_output * _weights.at(_w_r_index);
    if(_trace) std::cout << _previous_output << " x " << _weights.at(_w_r_index) << std::endl;

    reset_input_sum += 1 * _weights.at(_b_r_index);
    if(_trace) std::cout << "bias: 1 x " << _weights.at(_b_r_index) << std::endl;

    if(_trace) std::cout << "reset_input_sum: " << reset_input_sum << std::endl;

    //Reset gate output
    //NOTE: NEAT-GRU code uses K value of 1/4.924273 here
    double r = _layer_spec._activation_func->activate(reset_input_sum);
    if(_trace) std::cout << "r: " << r << std::endl;

    //Update gate
    double update_input_sum = 0.0;

    for(unsigned i = 0; i < _layer_spec._inputs_per_neuron; i++) 
    {
        update_input_sum += inputs.at(i) * _weights.at(_U_u_index + i);
        if(_trace) std::cout << inputs.at(i) << " x " << _weights.at(_U_u_index + i) << std::endl;
    }

    update_input_sum += _previous_output * _weights.at(_w_u_index);
    if(_trace) std::cout << _previous_output << " x " << _weights.at(_w_u_index) << std::endl;

    update_input_sum += 1 * _weights.at(_b_u_index);
    if(_trace) std::cout << "bias: 1 x " << _weights.at(_b_u_index) << std::endl;

    if(_trace) std::cout << "update_input_sum: " << update_input_sum << std::endl;

    //Update gate output
    double u = _layer_spec._activation_func->activate(update_input_sum);
    if(_trace) std::cout << "u: " << u << std::endl;

    /* Calculate h_tilda */
    double reset_mult = r * _weights.at(_w_index) * _previous_output;
    if(_trace) std::cout << r << " x " << _weights.at(_w_index) << " x " << _previous_output << 
                   std::endl;
    if(_trace) std::cout << "Reset mult: " << reset_mult << std::endl;

    //tanh gate
    double tanh_input_sum = 0.0;

    for(unsigned i = 0; i < _layer_spec._inputs_per_neuron; i++) 
    {
        tanh_input_sum += inputs.at(i) * _weights.at(_U_index + i);
        if(_trace) std::cout << inputs.at(i) << " x " << _weights.at(_U_index + i) << std::endl;
    }

    tanh_input_sum += reset_mult + _weights.at(_b_index);
    if(_trace) std::cout << reset_mult << " + " << _weights.at(_b_index) << std::endl;
    if(_trace) std::cout << "tanh_input_sum: " << tanh_input_sum << std::endl;

    double h_tilda = tanh(tanh_input_sum);
    if(_trace) std::cout << "h_tilda: " << h_tilda << std::endl;

    //Final computation steps
    double h_tilda_mult = h_tilda * (1 - u);
    if(_trace) std::cout << h_tilda << " * (1 - " << u << ")" << std::endl;
    if(_trace) std::cout << "h_tilda_mutl: " << h_tilda_mult << std::endl;

    double u_history = u * _previous_output;
    if(_trace) std::cout << u << " x " << _previous_output << std::endl;
    if(_trace) std::cout << "u_history: " << u_history << std::endl;

    double output = h_tilda_mult + u_history;
    if(_trace) std::cout << h_tilda_mult << " + " << u_history << std::endl;
    if(_trace) std::cout << "Output: " << output << std::endl;

    if(_trace) std::cout << "---------" << std::endl;

    _previous_output = output;

    return output;

}

void GRUNeuron::reset() 
{
    _previous_output = 0.5;
}

} // namespace NeuroEvo
