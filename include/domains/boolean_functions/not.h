#ifndef _NOT_H_
#define _NOT_H_

#include <domains/domain.h>

class NOT : public Domain {

public:

    NOT(const bool DOMAIN_TRACE, const double COMPLETION_FITNESS = 1.99);

    bool check_phenotype_spec(PhenotypeSpec& pheno_spec) override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

};

#endif
