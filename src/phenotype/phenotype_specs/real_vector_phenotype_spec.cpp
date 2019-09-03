#include <phenotype/phenotype_specs/real_vector_phenotype_spec.h>
#include <genotype/real_vector_genotype.h>
#include <phenotype/real_vector_phenotype.h>
#include <gp_map/matrix_map.h>

namespace NeuroEvo {
namespace Phenotypes {

RealVectorPhenotypeSpec::RealVectorPhenotypeSpec(const unsigned int NUM_GENES) :
    PhenotypeSpec(NUM_GENES) {}

Phenotypes::Phenotype* RealVectorPhenotypeSpec::generate_phenotype(Genotypes::Genotype& genotype,
                                                                   GPMaps::GPMap* gp_map) {

    Genotypes::RealVectorGenotype* real_vec_genotype =
        dynamic_cast<Genotypes::RealVectorGenotype*>(&genotype);

    if(gp_map)
        return gp_map->map(*real_vec_genotype, *this);
    else
        return new Phenotypes::RealVectorPhenotype(real_vec_genotype->get_genes());

}

} // namespace Phenotypes
} // namespace NeuroEvo
