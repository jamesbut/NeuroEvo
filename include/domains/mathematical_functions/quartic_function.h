#ifndef _QUARTIC_FUNCTION_H_
#define _QUARTIC_FUNCTION_H_

#include <domains/domain.h>

class QuarticFunction : public Domain {

public:

    QuarticFunction(const double A, const double B, const double C,
                    const double D, const double E,
                    const bool DOMAIN_TRACE, const double COMPLETION_FITNESS = 0.0);

    bool check_phenotype_spec(PhenotypeSpec& pheno_spec) override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

    const double _A;
    const double _B;
    const double _C;
    const double _D;
    const double _E;

};

#endif
