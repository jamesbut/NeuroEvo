#ifndef _GRU_NEURON_H_
#define _GRU_NEURON_H_

#include <phenotype/fixed_network/neuron.h>

class GRUNeuron : public Neuron {

public:

    GRUNeuron(const LayerSpec& LAYER_SPEC, const bool TRACE);

    void set_weights(std::vector<double>& weights) override;
    double evaluate(std::vector<double>& inputs) override;

protected:

    Neuron* clone_impl() const override { return new GRUNeuron(*this); };

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

#endif
