#ifndef _DOUBLE_TO_DOUBLE_NET_MAP_
#define _DOUBLE_TO_DOUBLE_NET_MAP_

#include <gp_map/network_maps/network_map.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>

/*
 * Defines a network decoder where the input and output of the decoder
 * are vectors of doubles
 */

namespace NeuroEvo {

class DoubleToDoubleNetMap : public NetworkMap<double, double>
{

public:

    DoubleToDoubleNetMap(NetworkBuilder& decoder_spec,
                         std::shared_ptr<VectorPhenotypeSpec> pheno_spec);
    DoubleToDoubleNetMap(const JSON& json);

    Phenotype<double>* map(Genotype<double>& genotype) override;

private:

    JSON to_json_impl() const override;
    DoubleToDoubleNetMap* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
