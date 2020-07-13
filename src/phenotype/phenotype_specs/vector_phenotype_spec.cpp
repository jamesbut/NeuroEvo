#include <phenotype/phenotype_specs/vector_phenotype_spec.h>

namespace NeuroEvo {

VectorPhenotypeSpec::VectorPhenotypeSpec(const unsigned num_genes) :
    PhenotypeSpec(num_genes) {}


VectorPhenotypeSpec* VectorPhenotypeSpec::clone_impl() const
{
    return new VectorPhenotypeSpec(*this);
};

} // namespace NeuroEvo
