#ifndef _NAND_H_
#define _NAND_H_

#include <domains/domain.h>

namespace NeuroEvo {
namespace Domains {

class NAND : public Domain {

public:

    NAND(const bool DOMAIN_TRACE, const double COMPLETION_FITNESS = 3.9);

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

};

} // namespace Domains
} // namespace NeuroEvo

#endif
