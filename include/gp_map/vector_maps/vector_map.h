#ifndef _VECTOR_MAP_
#define _VECTOR_MAP_

/*
 * The default vector map that maps a genotype to a vector phenotype both of the same type
 */

#include <gp_map/gp_map.h>
#include <phenotype/vector_phenotype.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>

namespace NeuroEvo {

template <typename G>
class VectorMap : public GPMap<G, G>
{

public:

    VectorMap(std::shared_ptr<VectorPhenotypeSpec> pheno_spec) :
        GPMap<G, G>(pheno_spec) {}

    VectorMap(const JSON& json) :
        VectorMap(
            std::make_shared<VectorPhenotypeSpec>(JSON(json.at({"PhenotypeSpec"})))) {}

    Phenotype<G>* map(Genotype<G>& genotype) override
    {
        const std::vector<G> traits(genotype.genes().begin(), genotype.genes().end());
        return new VectorPhenotype<G>(traits);
    }

    void print(std::ostream& os) const override {}

private:

    JSON to_json_impl() const override
    {
        JSON json;
        json.emplace("name", "VectorMap");
        return json;
    }

    VectorMap* clone_impl() const override
    {
        return new VectorMap<G>(*this);
    }

};

static Factory<GPMap<double, double>>::Registrar vector_map_registrar(
    "VectorMap",
    [](const JSON& json) {return std::make_shared<VectorMap<double>>(json);});

} // namespace NeuroEvo

#endif

