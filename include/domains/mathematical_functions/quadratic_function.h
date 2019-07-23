#ifndef _QUADRATIC_FUNCTION_H_
#define _QUADRATIC_FUNCTION_H_

#include <domains/domain.h>
#include <population.h>

class QuadraticFunction : public Domain {

public:

    QuadraticFunction(const double A, const double B, const double C,
                      const bool DOMAIN_TRACE, const double COMPLETION_FITNESS = 0.0);

    bool check_phenotype_spec(PhenotypeSpec& pheno_spec) override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

    const double _A;
    const double _B;
    const double _C;

};

#endif
