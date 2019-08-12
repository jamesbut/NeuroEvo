#ifndef _NOR_H_
#define _NOR_H_

#include <domains/domain.h>

namespace NeuroEvo {
namespace Domains {

class NOR : public Domain {

public:

    NOR(const bool DOMAIN_TRACE, const double COMPLETION_FITNESS = 3.9);

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

};

} // namespace Domains
} // namesapce NeuroEvo

#endif
