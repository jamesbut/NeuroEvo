#ifndef _QUARTIC_FUNCTION_H_
#define _QUARTIC_FUNCTION_H_

#include <domains/domain.h>

namespace NeuroEvo {

class QuarticFunction : public Domain {

public:

    QuarticFunction(const double a, const double b, const double c,
                    const double d, const double e,
                    const bool domain_trace, const double completion_fitness = 0.0) :
        _a(a),
        _b(b),
        _c(c),
        _d(d),
        _e(e),
        Domain(domain_trace, completion_fitness) {}

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override {

        Phenotypes::RealVectorPhenotypeSpec* real_vec_pheno_spec;
        real_vec_pheno_spec = dynamic_cast<Phenotypes::RealVectorPhenotypeSpec*>(&pheno_spec);

        if(real_vec_pheno_spec == nullptr) {

            std::cerr << "Only real vector phenotype specifications are allowed" <<
                        "with the quartic domain!" << std::endl;

            return false;

        }

        if(real_vec_pheno_spec->get_num_params() != 1) {

            std::cerr << "The number of genes needs to equal 1 for a 1-dimensional function!" << 
                std::endl;

            return false;

        }

        return true;

    }

private:

    double single_run(Organism& org, unsigned rand_seed) override {

        //There are no inputs and outputs for a mathematical function
        //The output is just the value of the gene itself
        std::vector<double> inputs = std::vector<double>();
        std::vector<double> output = org.get_phenotype().activate(inputs);

        const double x = output.at(0);
        const double y = _a * pow(x, 4) + _b * pow(x, 3) + _c * pow(x, 2) + _d * x + _e;

        return y;

    }

    const double _a;
    const double _b;
    const double _c;
    const double _d;
    const double _e;

};

} // namespace NeuroEvo

#endif
