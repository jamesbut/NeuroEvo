#ifndef _DCT_MAP_SPEC_
#define _DCT_MAP_SPEC_

#include <gp_map/gp_map_specs/gp_map_spec.h>
#include <gp_map/dct_map.h>

namespace NeuroEvo {
namespace GPMaps {

struct DCTMapSpec : GPMapSpec {

public:

    DCTMapSpec(const unsigned C, const unsigned NUM_NEURONS, const unsigned INPUTS_PER_NEURON) :
        C(C),
        NUM_NEURONS(NUM_NEURONS),
        INPUTS_PER_NEURON(INPUTS_PER_NEURON) {}

    GPMap* generate_gp_map() override {

        return new DCTMap(C, NUM_NEURONS, INPUTS_PER_NEURON);

    }

    GPMap* generate_gp_map(const std::string& file_name) override {

        //Nothing has to be read from file here
        //The genes are the coefficients of the inverse DCT
        return new DCTMap(C, NUM_NEURONS, INPUTS_PER_NEURON);

    }

private:

    const unsigned C;
    const unsigned NUM_NEURONS;
    const unsigned INPUTS_PER_NEURON;

};

} // namespace GPMaps
} // namespace NeuroEvo

#endif
