#ifndef _AND_H_
#define _AND_H_

#include <domains/domain.h>

namespace NeuroEvo {
namespace Domains {

class AND : public Domain {

public:

    AND(const bool DOMAIN_TRACE, const double COMPLETION_FITNESS = 3.9);

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override;

protected: 

    void render() override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

};

} // namespace Domains
} // namespace NeuroEvo

#endif
