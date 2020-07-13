#ifndef _GP_MAP_
#define _GP_MAP_

/*
    A GPMap transforms a genotype into a phenotype by using a phenotype specification
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

    //I assume the phenotype specification will not change throughout an experiment
    GPMap(const PhenotypeSpec* pheno_spec) :
        _pheno_spec(pheno_spec) {}

    GPMap(const GPMap& gpmap) :
        _pheno_spec(gpmap._pheno_spec->clone()) {}

    virtual ~GPMap() = default;

    virtual Phenotype<T>* map(Genotype<G>& genotype) = 0;

    const PhenotypeSpec* get_pheno_spec() const
    {
        return _pheno_spec.get();
    }

    virtual void print_gp_map(std::ofstream& file) const = 0;

    auto clone() const 
    { 
        return std::unique_ptr<GPMap>(clone_impl()); 
    }


protected:

    virtual GPMap* clone_impl() const = 0;

    const std::unique_ptr<const PhenotypeSpec> _pheno_spec;

};

} // namespace NeuroEvo

#endif
