#ifndef _NETWORK_BASE_H_
#define _NETWORK_BASE_H_

/*
 * A base class for both the torch neural network and the custom neural network
 */

#include <phenotype/phenotype.h>
#include <util/maths/activation_functions/activation_function.h>

namespace NeuroEvo {

class NetworkBase : public Phenotype<double>
{

public:

    NetworkBase(const bool trace);

    unsigned get_num_inputs() const;
    unsigned get_num_outputs() const;

    std::shared_ptr<ActivationFunction> get_final_layer_activ_func() const;

protected:

    unsigned _num_inputs;
    unsigned _num_outputs;

   std::shared_ptr<ActivationFunction> _final_layer_activ_func;

};

} // namespace NeuroEvo

#endif
