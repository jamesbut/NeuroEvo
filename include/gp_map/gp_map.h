#ifndef _GP_MAP_
#define _GP_MAP_

/*
    A GPMap transforms a genotype into a phenotype.
*/

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <util/maths/matrix.h>
#include <optional>

namespace NeuroEvo {

template <typename G, typename T>
class GPMap 
{

public:

    virtual Phenotype<T>* map(Genotype<G>& genotype,
                              PhenotypeSpec<G, T>& pheno_spec) = 0;

    virtual ~GPMap() = default;

    virtual void print_gp_map(std::ofstream& file) const = 0;

    auto clone() const 
    { 
        return std::unique_ptr<GPMap>(clone_impl()); 
    }


protected:

    virtual GPMap* clone_impl() const = 0;

};

} // namespace NeuroEvo

#endif
