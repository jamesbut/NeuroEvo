#ifndef _GP_MAP_
#define _GP_MAP_

/*
    A GPMap transforms a genotype into a phenotype.
*/

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <util/maths/matrix.h>

// I would eventually like to sort out this circular
// dependency
struct PhenotypeSpec;

class GPMap {

public:

    virtual Phenotype* map(Genotype& genotype, PhenotypeSpec& pheno_spec) = 0;

    virtual ~GPMap() = default;

    virtual void print_gp_map(std::ofstream& file) = 0;

    // Only supports a Matrix of doubles as a map
    // I will make this generic in future
    virtual Matrix<double>& get_map() = 0;

    auto clone() const { return std::unique_ptr<GPMap>(clone_impl()); }

protected:

    virtual GPMap* clone_impl() const = 0;

};

#endif
