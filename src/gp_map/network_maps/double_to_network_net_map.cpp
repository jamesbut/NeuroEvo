#include <gp_map/network_maps/double_to_network_net_map.h>

namespace NeuroEvo {

DoubleToNetworkNetMap::DoubleToNetworkNetMap(NetworkBuilder& decoder_spec,
                                             std::shared_ptr<NetworkBuilder> pheno_spec) :
    NetworkMap<double, double>(decoder_spec, pheno_spec) {}

Phenotype<double>* DoubleToNetworkNetMap::map(Genotype<double>& genotype)
{
    //Push genotype through the decoder
    const std::vector<double> decoder_output = _decoder->activate(genotype.genes());

    //Set network weights to decoder output
    NetworkBuilder* pheno_net_builder = dynamic_cast<NetworkBuilder*>(_pheno_spec.get());
    pheno_net_builder->set_init_weights(decoder_output);
    auto pheno = pheno_net_builder->build_network();
    return pheno;
}

DoubleToNetworkNetMap* DoubleToNetworkNetMap::clone_impl() const
{
    return new DoubleToNetworkNetMap(*this);
}

} // namespace NeuroEvo
