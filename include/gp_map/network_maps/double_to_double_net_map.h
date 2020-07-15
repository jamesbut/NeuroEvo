#ifndef _DOUBLE_TO_DOUBLE_NET_MAP_
#define _DOUBLE_TO_DOUBLE_NET_MAP_

#include <gp_map/network_maps/network_map.h>

/*
 * Defines a network decoder where the input and output of the decoder
 * are vectors of doubles
 */

namespace NeuroEvo {

class DoubleToDoubleNetMap : public NetworkMap<double, double>
{

public:

    DoubleToDoubleNetMap(NetworkBuilder& decoder_spec, const PhenotypeSpec* pheno_spec);

    Phenotype<double>* map(Genotype<double>& genotype) override;

private:

    DoubleToDoubleNetMap* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
