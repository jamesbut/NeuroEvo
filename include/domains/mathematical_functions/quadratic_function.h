#ifndef _QUADRATIC_FUNCTION_H_
#define _QUADRATIC_FUNCTION_H_

#include <domains/domain.h>
#include <population.h>

namespace NeuroEvo {

template <typename G>
class QuadraticFunction : public Domain<G, double> 
{

public:

    QuadraticFunction(const double a, const double b, const double c,
                      const bool domain_trace, const double completion_fitness = 0.0) :
        _a(a),
        _b(b),
        _c(c),
        Domain<G, double>(domain_trace, completion_fitness) {}

    bool check_phenotype_spec(PhenotypeSpec<G, double>& pheno_spec) override 
    {

        VectorPhenotypeSpec<double>* real_vec_pheno_spec = 
            dynamic_cast<VectorPhenotypeSpec<double>*>(&pheno_spec);

        if(real_vec_pheno_spec == nullptr) 
        {
            std::cerr << "Only vector phenotype specifications are allowed" <<
                        "with the quadratic domain!" << std::endl;
            return false;
        }

        if(real_vec_pheno_spec->get_num_params() != 1) 
        {
            std::cerr << "The number of params needs to equal 1 for a 1-dimensional function!" << 
                std::endl;
            return false;
        }

        return true;

    }

private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override 
    {

        //There are no inputs and outputs for a mathematical function
        //The output is just the value of the gene itself
        std::vector<double> inputs = std::vector<double>();
        std::vector<double> output = org.get_phenotype().activate(inputs);

        const double x = output.at(0);
        const double y = _a * pow(x, 2) + _b * x + _c;

        return y;

    }

    void render() override {}
    void reset_domain() override {}

    const double _a;
    const double _b;
    const double _c;

};

} // namespace NeuroEvo

#endif
