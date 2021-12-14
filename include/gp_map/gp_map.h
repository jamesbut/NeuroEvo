#ifndef _GP_MAP_
#define _GP_MAP_

/*
    A GPMap transforms a genotype into a phenotype by using a phenotype specification
*/

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <util/maths/matrix.h>
#include <optional>
#include <util/factory.h>

namespace NeuroEvo {

template <typename G, typename T>
class GPMap
{

public:

    //Do not implement default constructor! We always want _pheno_spec set to something

    //I assume the phenotype specification will not change throughout an experiment
    GPMap(std::shared_ptr<PhenotypeSpec> pheno_spec) :
        _pheno_spec(pheno_spec) {}

    GPMap(const GPMap& gpmap) :
        _pheno_spec(gpmap._pheno_spec->clone()) {}

    virtual ~GPMap() = default;

    virtual Phenotype<T>* map(Genotype<G>& genotype) = 0;

    const std::shared_ptr<PhenotypeSpec>& get_pheno_spec() const
    {
        return _pheno_spec;
    }

    void set_pheno_spec_trace(const bool trace)
    {
        _pheno_spec->set_trace(trace);
    }

    virtual void print(std::ostream& os) const = 0;

    auto clone() const
    {
        return std::unique_ptr<GPMap>(clone_impl());
    }


protected:

    virtual GPMap* clone_impl() const = 0;

    const std::shared_ptr<PhenotypeSpec> _pheno_spec;

};

} // namespace NeuroEvo

#endif
