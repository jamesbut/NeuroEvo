#ifndef _VECTOR_MAP_
#define _VECTOR_MAP_

/*
 * The default vector map that maps a genotype to a vector phenotype both of the same type
 */

#include <gp_map/gp_map.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

template <typename G>
class VectorMap : public GPMap<G, G>
{

public:

    VectorMap(const VectorPhenotypeSpec* pheno_spec) :
        GPMap<G, G>(pheno_spec) {}

    Phenotype<G>* map(Genotype<G>& genotype) override
    {
        const std::vector<G> traits(genotype.genes().begin(), genotype.genes().end());
        return new VectorPhenotype<G>(traits);
    }

    void print_gp_map(std::ofstream& file) const override {}

protected:

    VectorMap* clone_impl() const override 
    {
        return new VectorMap<G>(*this);
    }

};

} // namespace NeuroEvo

#endif

