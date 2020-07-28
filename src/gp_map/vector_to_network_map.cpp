#include <gp_map/vector_to_network_map.h>

namespace NeuroEvo {

VectorToNetworkMap::VectorToNetworkMap(NetworkBuilder* net_builder) :
    GPMap<double, double>(net_builder) {}

Phenotype<double>* VectorToNetworkMap::map(Genotype<double>& genotype)
{
    const NetworkBuilder* net_builder_cast = 
        dynamic_cast<const NetworkBuilder*>(_pheno_spec.get()); 

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

} // namespace NeuroEvo
