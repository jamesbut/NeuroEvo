#include <gp_map/network_maps/bool_to_bool_net_map.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

BoolToBoolNetMap::BoolToBoolNetMap(NetworkBuilder& net_builder,
                                   std::shared_ptr<VectorPhenotypeSpec> pheno_spec) :
    NetworkMap<bool, bool>(net_builder, pheno_spec) {}

Phenotype<bool>* BoolToBoolNetMap::map(Genotype<bool>& genotype)
{
    //Push genotype through decoder
    const std::vector<double> double_genotype(genotype.genes().begin(),
                                              genotype.genes().end());
    const std::vector<double> decoder_output = _decoder->activate(double_genotype);

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

JSON BoolToBoolNetMap::to_json_impl() const
{
    JSON json;
    json.emplace("name", "BoolToBoolNetMap");
    json.emplace("NetworkBuilder", _decoder->to_json());
    return json;
}

BoolToBoolNetMap* BoolToBoolNetMap::clone_impl() const
{
    return new BoolToBoolNetMap(*this);
}

} // namespace NeuroEvo
