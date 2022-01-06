#include <gp_map/network_maps/double_to_double_net_map.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

DoubleToDoubleNetMap::DoubleToDoubleNetMap(
    NetworkBuilder& net_builder,
    std::shared_ptr<VectorPhenotypeSpec> pheno_spec) :
    NetworkMap<double, double>(net_builder, pheno_spec) {}

DoubleToDoubleNetMap::DoubleToDoubleNetMap(const JSON& json) :
    NetworkMap<double, double>(json) {}

Phenotype<double>* DoubleToDoubleNetMap::map(Genotype<double>& genotype)
{
    //Push genotype through decoder
    const std::vector<double> decoder_output = _decoder->activate(genotype.genes());

    //The output of the decoder are the traits of the returned vector
    return new VectorPhenotype<double>(decoder_output);
}

JSON DoubleToDoubleNetMap::to_json_impl() const
{
    JSON json;
    json.emplace("name", "DoubleToDoubleNetMap");
    json.emplace("NetworkBuilder", _decoder->to_json());
    return json;
}

DoubleToDoubleNetMap* DoubleToDoubleNetMap::clone_impl() const
{
    return new DoubleToDoubleNetMap(*this);
}

static Factory<GPMap<double, double>>::Registrar double_to_double_net_map_registrar(
    "DoubleToDoubleNetMap",
    [](const JSON& json) {return std::make_shared<DoubleToDoubleNetMap>(json);});

} // namespace NeuroEvo
