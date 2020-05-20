#ifndef _QUADRATIC_FUNCTION_H_
#define _QUADRATIC_FUNCTION_H_

#include <domains/domain.h>
#include <population.h>

namespace NeuroEvo {

class QuadraticFunction : public Domain {

public:

    QuadraticFunction(const double a, const double b, const double c,
                      const bool domain_trace, const double completion_fitness = 0.0) :
        _a(a),
        _b(b),
        _c(c),
        Domain(domain_trace, completion_fitness) {}

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override {

        Phenotypes::RealVectorPhenotypeSpec* real_vec_pheno_spec;
        real_vec_pheno_spec = dynamic_cast<Phenotypes::RealVectorPhenotypeSpec*>(&pheno_spec);

        if(real_vec_pheno_spec == nullptr) {

            std::cerr << "Only real vector phenotype specifications are allowed" <<
                        "with the quadratic domain!" << std::endl;

            return false;

        }

        if(real_vec_pheno_spec->get_num_params() != 1) {

            std::cerr << "The number of params needs to equal 1 for a 1-dimensional function!" << 
                std::endl;

            return false;

        }

        return true;

    }

protected:

    void render() override {}

private:

    double single_run(Organism& org, unsigned rand_seed) override {

            //There are no inputs and outputs for a mathematical function
            //The output is just the value of the gene itself
            std::vector<double> inputs = std::vector<double>();
            std::vector<double> output = org.get_phenotype().activate(inputs);

            const double x = output.at(0);
            const double y = _a * pow(x, 2) + _b * x + _c;

            return y;

    }

    const double _a;
    const double _b;
    const double _c;

};

} // namespace NeuroEvo

#endif
