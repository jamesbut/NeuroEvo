#ifndef _N_LEGGED_TABLE_H_
#define _N_LEGGED_TABLE_H_

/*
    The n-legged table domain is a task where the aim
    is to optimise the height of a table with n legs
    whilst keeping all the legs similar size.
    This is a useful domain for testing evolvability.
*/

#include <domains/domain.h>

namespace NeuroEvo {
namespace Domains {

class NLeggedTable : public Domain {

public:

    NLeggedTable(const unsigned NUM_LEGS, const bool DOMAIN_TRACE = false);

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

    const unsigned _NUM_LEGS;

};

} // namespace Domains
} // namespace NeuroEvo

#endif
