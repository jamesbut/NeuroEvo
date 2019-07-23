#include <phenotype/phenotype_specs/real_vector_phenotype_spec.h>
#include <genotype/real_vector_genotype.h>
#include <phenotype/real_vector_phenotype.h>
#include <gp_map/matrix_map.h>

RealVectorPhenotypeSpec::RealVectorPhenotypeSpec(const unsigned int NUM_GENES) :
    PhenotypeSpec(NUM_GENES) {}

Genotype* RealVectorPhenotypeSpec::generate_genotype() {

    return new RealVectorGenotype(NUM_GENES);

}

Genotype* RealVectorPhenotypeSpec::generate_genotype(Genotype& genotype) {

    RealVectorGenotype* real_vec_genotype = dynamic_cast<RealVectorGenotype*>(&genotype);

    return new RealVectorGenotype(*real_vec_genotype);

}

Genotype* RealVectorPhenotypeSpec::generate_genotype(const std::string& file_name) {

    return new RealVectorGenotype(file_name);

}

GPMap* RealVectorPhenotypeSpec::generate_gp_map() {

    return new MatrixMap(NUM_GENES, NUM_GENES);

}

GPMap* RealVectorPhenotypeSpec::generate_gp_map(const std::string& file_name) {

    return new MatrixMap(NUM_GENES, NUM_GENES, file_name);

}

Phenotype* RealVectorPhenotypeSpec::generate_phenotype(Genotype& genotype,
                                                       GPMap* gp_map) {

    RealVectorGenotype* real_vec_genotype = dynamic_cast<RealVectorGenotype*>(&genotype);

    if(gp_map)
        return gp_map->map(*real_vec_genotype, *this);
    else
        return new RealVectorPhenotype(real_vec_genotype->get_genes());

}
