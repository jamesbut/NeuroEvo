#ifndef _VECTOR_TO_NETWORK_MAP_H_
#define _VECTOR_TO_NETWORK_MAP_H_

/*
 * This map just defines how to set the genes of a double genotype as the weights for a network
 */

#include <gp_map/gp_map.h>
#include <phenotype/phenotype_specs/network_builder.h>

namespace NeuroEvo {

class VectorToNetworkMap : public GPMap<double, double>
{

public:

    VectorToNetworkMap(NetworkBuilder* net_builder);

    Phenotype<double>* map(Genotype<double>& genotype) override;

    void print(std::ostream& os) const override {}

protected:

    VectorToNetworkMap* clone_impl() const override
    {
        return new VectorToNetworkMap(*this);
    }

};

} // namespace NeuroEvo

#endif
