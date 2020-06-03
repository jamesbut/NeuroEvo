#ifndef _DCT_MAP_SPEC_
#define _DCT_MAP_SPEC_

#include <gp_map/gp_map_specs/gp_map_spec.h>
#include <gp_map/dct_map.h>

namespace NeuroEvo {

class DCTMapSpec : public GPMapSpec<double, double> 
{

public:

    DCTMapSpec(const unsigned c, const unsigned num_neurons, const unsigned inputs_per_neuron) :
        _c(c),
        _num_neurons(num_neurons),
        _inputs_per_neuron(inputs_per_neuron) {}

    GPMap<double, double>* generate_gp_map() override 
    {
        return new DCTMap(_c, _num_neurons, _inputs_per_neuron);
    }

    GPMap<double, double>* generate_gp_map(const std::string& file_name) override 
    {
        //Nothing has to be read from file here
        //The genes are the coefficients of the inverse DCT
        return new DCTMap(_c, _num_neurons, _inputs_per_neuron);
    }

private:

    const unsigned _c;
    const unsigned _num_neurons;
    const unsigned _inputs_per_neuron;

};

} // namespace NeuroEvo

#endif
