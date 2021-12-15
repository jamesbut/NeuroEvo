#ifndef _VECTOR_PHENOTYPE_SPEC_H_
#define _VECTOR_PHENOTYPE_SPEC_H_

#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

class VectorPhenotypeSpec : public PhenotypeSpec
{

public:

    VectorPhenotypeSpec(const unsigned num_genes);
    VectorPhenotypeSpec(const JSON& json);

private:

    JSON to_json_impl() const override;
    VectorPhenotypeSpec* clone_impl() const override;

};

} // namespace NeuroEvo

#endif
