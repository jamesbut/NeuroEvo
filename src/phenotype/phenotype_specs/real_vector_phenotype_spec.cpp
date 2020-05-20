#include <phenotype/phenotype_specs/real_vector_phenotype_spec.h>
#include <genotype/real_vector_genotype.h>
#include <phenotype/real_vector_phenotype.h>
#include <gp_map/matrix_map.h>

namespace NeuroEvo {

RealVectorPhenotypeSpec::RealVectorPhenotypeSpec(const unsigned num_genes) :
    PhenotypeSpec(num_genes) {}

Phenotype* RealVectorPhenotypeSpec::generate_phenotype(Genotype<double>& genotype,
                                                       GPMap<double>* gp_map) 
{

    RealVectorGenotype* real_vec_genotype =
        dynamic_cast<RealVectorGenotype*>(&genotype);

    if(gp_map)
        return gp_map->map(*real_vec_genotype, *this);
    else
        return new RealVectorPhenotype(real_vec_genotype->get_genes());

}

} // namespace NeuroEvo
