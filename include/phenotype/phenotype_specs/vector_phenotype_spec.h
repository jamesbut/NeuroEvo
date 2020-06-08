#ifndef _VECTOR_PHENOTYPE_SPEC_H_
#define _VECTOR_PHENOTYPE_SPEC_H_

#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

//This class describes how to build a generic VectorPhenotype from a generic 
//genotype and gp map - right now the genes and traits must match type
template <typename G>
class VectorPhenotypeSpec : public PhenotypeSpec<G, G> {

public:

    VectorPhenotypeSpec(const unsigned num_genes) :
        PhenotypeSpec<G, G>(num_genes) {}

    Phenotype<G>* generate_phenotype(Genotype<G>& genotype,
                                     GPMap<G, G>* gp_map) override
    {
        Phenotype<G>* phenotype;
        if(gp_map)
            phenotype = gp_map->map(genotype, *this);
        else
            phenotype = new VectorPhenotype<G>(genotype.genes());

        //Check phenotype is of the correct size
        if(phenotype->get_num_params().value() != this->_num_params)
        {
            std::cerr << "Created phenotype does not have the same number of params as"
                << " specified in the phenotype spec!" << std::endl;
            std::cerr << "Num params in phenotype spec: " << this->_num_params << std::endl;
            std::cerr << "Actual number of params in phenotype: " << 
                phenotype->get_num_params().value() << std::endl;
            exit(0);
        }

        return phenotype;

    }

protected:

    VectorPhenotypeSpec* clone_impl() const override 
    {
        return new VectorPhenotypeSpec(*this);
    };

};

} // namespace NeuroEvo

#endif
