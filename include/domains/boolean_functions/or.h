#ifndef _OR_H_
#define _OR_H_

#include <domains/domain.h>

class OR : public Domain {

public:

    OR(const bool DOMAIN_TRACE, const double COMPLETION_FITNESS = 3.9);

    bool check_phenotype_spec(PhenotypeSpec& pheno_spec) override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

};

#endif
