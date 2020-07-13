#ifndef _BOOL_TO_BOOL_NET_MAP_
#define _BOOL_TO_BOOL_NET_MAP_

#include <gp_map/network_maps/network_map.h>

namespace NeuroEvo {

class BoolToBoolNetMap : public NetworkMap<bool, bool>
{

public:

    BoolToBoolNetMap(NetworkBuilder& decoder_spec, const PhenotypeSpec* pheno_spec);

    Phenotype<bool>* map(Genotype<bool>& genotype) override;

private:

    BoolToBoolNetMap* clone_impl() const override;

};

} // namespace NeuroEvo

#endif

