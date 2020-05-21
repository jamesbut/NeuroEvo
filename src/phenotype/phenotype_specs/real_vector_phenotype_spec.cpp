#include <phenotype/phenotype_specs/real_vector_phenotype_spec.h>
#include <genotype/genotype.h>
#include <phenotype/real_vector_phenotype.h>
#include <gp_map/matrix_map.h>

namespace NeuroEvo {

RealVectorPhenotypeSpec::RealVectorPhenotypeSpec(const unsigned num_genes) :
    PhenotypeSpec(num_genes) {}

Phenotype* RealVectorPhenotypeSpec::generate_phenotype(Genotype<double>& genotype,
                                                       GPMap<double>* gp_map) 
{

    if(gp_map)
        return gp_map->map(genotype, *this);
    else
        return new RealVectorPhenotype(genotype.genes());

}

} // namespace NeuroEvo
