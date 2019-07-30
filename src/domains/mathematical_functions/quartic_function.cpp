#include <domains/mathematical_functions/quartic_function.h>
#include <phenotype/phenotype_specs/real_vector_phenotype_spec.h>
#include <cmath>
#include <iostream>

namespace NeuroEvo {
namespace Domains {

QuarticFunction::QuarticFunction(const double A, const double B, const double C,
                                 const double D, const double E,
                                 const bool DOMAIN_TRACE, const double COMPLETION_FITNESS) :
    _A(A),
    _B(B),
    _C(C),
    _D(D),
    _E(E),
    Domain(DOMAIN_TRACE, COMPLETION_FITNESS) {}


double QuarticFunction::single_run(Organism& org, unsigned rand_seed) {

        //There are no inputs and outputs for a mathematical function
        //The output is just the value of the gene itself
        std::vector<double> inputs = std::vector<double>();
        std::vector<double> output = org.get_phenotype().activate(inputs);

        const double x = output.at(0);
        const double y = _A * pow(x, 4) + _B * pow(x, 3) + _C * pow(x, 2) + _D * x + _E;

        return y;

}

bool QuarticFunction::check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) {

    Phenotypes::RealVectorPhenotypeSpec* real_vec_pheno_spec;
    real_vec_pheno_spec = dynamic_cast<Phenotypes::RealVectorPhenotypeSpec*>(&pheno_spec);

    if(real_vec_pheno_spec == nullptr) {

        std::cerr << "Only real vector phenotype specifications are allowed" <<
                     "with the quartic domain!" << std::endl;

        return false;

    }

    if(real_vec_pheno_spec->NUM_GENES != 1) {

        std::cerr << "The number of genes needs to equal 1 for a 1-dimensional function!" << std::endl;

        return false;

    }

    return true;

}

} // namespace Domains
} // namespace NeuroEvo
