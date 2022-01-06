#ifndef _DOUBLE_TO_NETWORK_NET_MAP_H_
#define _DOUBLE_TO_NETWORK_NET_MAP_H_

/*
 * This map takes a vector of doubles, passes them through a decoder network and
 * produces a neural net as the phenotype whose weights are the output of the decoder.
 */

#include <gp_map/network_maps/network_map.h>

namespace NeuroEvo {

class DoubleToNetworkNetMap : public NetworkMap<double, double>
{

public:

    DoubleToNetworkNetMap(NetworkBuilder& decoder_spec,
                          std::shared_ptr<NetworkBuilder> pheno_spec);

    Phenotype<double>* map(Genotype<double>& genotype) override;

private:

    JSON to_json_impl() const override;
    DoubleToNetworkNetMap* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
