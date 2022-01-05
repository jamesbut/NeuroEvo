#include <gp_map/vector_to_network_map.h>

namespace NeuroEvo {

VectorToNetworkMap::VectorToNetworkMap(std::shared_ptr<NetworkBuilder> net_builder) :
    GPMap<double, double>(net_builder) {}

VectorToNetworkMap::VectorToNetworkMap(const JSON& json) :
    VectorToNetworkMap(std::make_shared<NetworkBuilder>(
                           json.get<JSON>({"PhenotypeSpec"}))) {}

Phenotype<double>* VectorToNetworkMap::map(Genotype<double>& genotype)
{
    NetworkBuilder* net_builder_cast = dynamic_cast<NetworkBuilder*>(_pheno_spec.get());

    if(net_builder_cast == nullptr)
    {
        std::cerr << "Must pass NetworkBuilder to VectorToNetworkMap map function" <<
            std::endl;
        exit(0);
    }

    //Set network weights as the genes
    net_builder_cast->set_init_weights(genotype.genes());
    auto network = net_builder_cast->build_network();
    return network;
}

JSON VectorToNetworkMap::to_json_impl() const
{
    JSON json;
    json.emplace("name", "VectorToNetworkMap");
    return json;
}

VectorToNetworkMap* VectorToNetworkMap::clone_impl() const
{
    return new VectorToNetworkMap(*this);
}

static Factory<GPMap<double, double>>::Registrar vector_to_network_map_registrar(
    "VectorToNetworkMap",
    [](const JSON& json) {return std::make_shared<VectorToNetworkMap>(json);});

} // namespace NeuroEvo
