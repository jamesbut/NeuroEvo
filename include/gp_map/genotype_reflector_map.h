#ifndef _GENOTYPE_REFLECTOR_MAP_H_
#define _GENOTYPE_REFLECTOR_MAP_H_

/*
 * A genotype reflector map takes a genotype and reflects it about the end
 * of its genes. This results in a phenotype that is twice the size of the
 * genotype and symmetric about its center.
 */

#include <gp_map/gp_map.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

template <typename G>
class GenotypeReflectorMap : public GPMap<G, G>
{

public:

    GenotypeReflectorMap() {}

    Phenotype<G>* map(Genotype<G>& genotype,
                      PhenotypeSpec<G, G>& pheno_spec) override 
    {
        const std::vector<G> first_traits = genotype.genes();

        //Copy genotype
        std::vector<G> second_traits(first_traits.begin(), first_traits.end()); 

        //Reverse
        std::reverse(second_traits.begin(), second_traits.end());

        //Concatenate
        std::vector<G> traits(first_traits.begin(), first_traits.end());
        traits.insert(traits.end(), second_traits.begin(), second_traits.end());

        return new VectorPhenotype<G>(traits);

    }

    std::optional<Matrix<double>> get_map() override 
    {
        std::cerr << "Implement get_map in GenotypeReflectorMap" << std::endl;
        exit(0);
        return std::nullopt;
    }

    void print(std::ostream& os) override {}

private:

    GenotypeReflectorMap<G>* clone_impl() const override 
    { 
        return new GenotypeReflectorMap<G>(*this); 
    }

};

} // namespace NeuroEvo

#endif
