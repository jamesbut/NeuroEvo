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

template <typename G>
class PhenotypeSpec;

template <typename G>
class GPMap 
{

public:

    virtual Phenotype* map(Genotype<G>& genotype,
                           PhenotypeSpec<G>& pheno_spec) = 0;

    virtual ~GPMap() = default;

    virtual void print_gp_map(std::ofstream& file) = 0;

    // The GPMap can be accessed and modified by the mutator functions
    // if it can be represented in vector or Matrix format.
    // If the map cannot be represented in Matrix format, the optional
    // should just be empty
    virtual std::optional<Utils::Matrix<double>> get_map() = 0;

    auto clone() const 
    { 
        return std::unique_ptr<GPMap>(clone_impl()); 
    }


protected:

    virtual GPMap* clone_impl() const = 0;

};

} // namespace NeuroEvo

#endif
