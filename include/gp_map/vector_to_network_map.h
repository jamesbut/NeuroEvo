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

    Phenotype<double>* map(Genotype<double>& genotype,
                           PhenotypeSpec<double, double>& net_builder) override
    {
        NetworkBuilder* net_builder_cast = dynamic_cast<NetworkBuilder*>(&net_builder); 

        if(net_builder_cast == nullptr)
        {
            std::cerr << "Must pass NetworkBuilder to VectorToNetworkMap map function" << 
                std::endl;
            exit(0);
        }

        //Set network weights as the genes
        auto network = net_builder_cast->build_network(genotype.genes());
        return network;

    }

    void print_gp_map(std::ofstream& file) const override {}

protected:

    VectorToNetworkMap* clone_impl() const override
    {
        return new VectorToNetworkMap(*this);
    }

};

} // namespace NeuroEvo

#endif
