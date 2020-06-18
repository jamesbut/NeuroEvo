#ifndef _GRU_NEURON_H_
#define _GRU_NEURON_H_

#include <phenotype/neural_network/neuron.h>

namespace NeuroEvo {

class GRUNeuron : public Neuron 
{

public:

    GRUNeuron(const unsigned num_inputs, 
              const std::unique_ptr<ActivationFunction>& activation_function,
              const bool trace);

    void set_weights(const std::vector<double>& weights) override;
    double evaluate(const std::vector<double>& inputs) override;

    void reset() override;

protected:

    Neuron* clone_impl() const override 
    { 
        return new GRUNeuron(*this); 
    };

private:

    //Points to param values in weights array.
    //These will depends on the number of inputs
    //so these have to be determined at run time.
    //I had problems using iterators here so I switched
    //to this method.
    unsigned _U_index;
    unsigned _U_r_index;
    unsigned _U_u_index;

    unsigned _w_index;
    unsigned _w_r_index;
    unsigned _w_u_index;
    unsigned _b_index;
    unsigned _b_r_index;
    unsigned _b_u_index;

};

} // namespace NeuroEvo

#endif
