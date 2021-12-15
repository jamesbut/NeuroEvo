#include <phenotype/phenotype_specs/vector_phenotype_spec.h>

namespace NeuroEvo {

VectorPhenotypeSpec::VectorPhenotypeSpec(const unsigned num_genes) :
    PhenotypeSpec(num_genes) {}

VectorPhenotypeSpec::VectorPhenotypeSpec(const JSON& json) :
    PhenotypeSpec(json) {}

JSON VectorPhenotypeSpec::to_json_impl() const
{
    JSON json;
    json.emplace("name", "VectorPhenotypeSpec");
    return json;
}

VectorPhenotypeSpec* VectorPhenotypeSpec::clone_impl() const
{
    return new VectorPhenotypeSpec(*this);
};

static Factory<PhenotypeSpec>::Registrar vector_pheno_spec_registrar(
    "VectorPhenotypeSpec",
    [](const JSON& json) {return std::make_shared<VectorPhenotypeSpec>(json);});

} // namespace NeuroEvo
