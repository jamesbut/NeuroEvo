#include <gp_map/network_maps/double_to_bool_net_map.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

DoubleToBoolNetMap::DoubleToBoolNetMap(NetworkBuilder& net_builder,
                                       VectorPhenotypeSpec* pheno_spec) :
    NetworkMap<double, bool>(net_builder, pheno_spec) {}

Phenotype<bool>* DoubleToBoolNetMap::map(Genotype<double>& genotype)
{
    //Push genotype through decoder
    const std::vector<double> decoder_output = _decoder->activate(genotype.genes());

    //Cast output back to bools
    std::vector<bool> traits(decoder_output.size());
    for(std::size_t i = 0; i < decoder_output.size(); i++)
    {
        if(decoder_output[i] < 0.5)
            traits[i] = false;
        else
            traits[i] = true;
    }

    //Only returns vector phenotypes for now
    return new VectorPhenotype<bool>(traits);

}

DoubleToBoolNetMap* DoubleToBoolNetMap::clone_impl() const
{
    return new DoubleToBoolNetMap(*this);
}

} // namespace NeuroEvo
