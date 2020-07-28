#ifndef _BOOL_TO_BOOL_NET_MAP_
#define _BOOL_TO_BOOL_NET_MAP_

#include <gp_map/network_maps/network_map.h>

/*
 * Defines a network decoder where the input and output of the decoder
 * are both boolean vectors 
 */

namespace NeuroEvo {

class BoolToBoolNetMap : public NetworkMap<bool, bool>
{

public:

    BoolToBoolNetMap(NetworkBuilder& decoder_spec, PhenotypeSpec* pheno_spec);

    Phenotype<bool>* map(Genotype<bool>& genotype) override;

private:

    BoolToBoolNetMap* clone_impl() const override;

};

} // namespace NeuroEvo

#endif

