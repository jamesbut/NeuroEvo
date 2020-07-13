#ifndef _DOUBLE_TO_BOOL_NET_MAP_
#define _DOUBLE_TO_BOOL_NET_MAP_

#include "phenotype/phenotype.h"
#include <gp_map/network_maps/network_map.h>

/*
 * Defines a network decoder where the input and output of the decoder
 * are double and boolean vectors respectively
 */

namespace NeuroEvo {

class DoubleToBoolNetMap : public NetworkMap<double, bool>
{

public:

    DoubleToBoolNetMap(NetworkBuilder& decoder_spec, const PhenotypeSpec* pheno_spec);

    Phenotype<bool>* map(Genotype<double>& genotype) override;

private:

    DoubleToBoolNetMap* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
