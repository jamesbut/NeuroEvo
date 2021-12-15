#include <gp_map/vector_maps/double_to_bool_vector_map.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

DoubleToBoolVectorMap::DoubleToBoolVectorMap(
    std::shared_ptr<VectorPhenotypeSpec> pheno_spec) :
    GPMap<double, bool>(pheno_spec) {}

//If the gene value is less than 0.5 then the trait is false and vice versa
Phenotype<bool>* DoubleToBoolVectorMap::map(Genotype<double>& genotype)
{
    std::vector<bool> traits(genotype.genes().size());

    for(std::size_t i = 0; i < traits.size(); i++)
        if(genotype.genes()[i] < 0.5)
            traits[i] = false;
        else
            traits[i] = true;

    return new VectorPhenotype<bool>(traits);
}

JSON DoubleToBoolVectorMap::to_json_impl() const
{
    JSON json;
    json.emplace("name", "DoubleToBoolVectorMap");
    return json;
}

DoubleToBoolVectorMap* DoubleToBoolVectorMap::clone_impl() const
{
    return new DoubleToBoolVectorMap(*this);
}

} // namespace NeuroEvo
