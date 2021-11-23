#ifndef _DOUBLE_TO_BOOL_VECTOR_MAP_
#define _DOUBLE_TO_BOOL_VECTOR_MAP_

/*
 * This map simply defines how to turn a genotype of doubles into a vector of bools
 */

#include <gp_map/gp_map.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>

namespace NeuroEvo {

class DoubleToBoolVectorMap : public GPMap<double, bool>
{

public:

    DoubleToBoolVectorMap(std::shared_ptr<VectorPhenotypeSpec> pheno_spec);

    Phenotype<bool>* map(Genotype<double>& genotype) override;

    void print(std::ostream& os) const override {}

protected:

    DoubleToBoolVectorMap* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
