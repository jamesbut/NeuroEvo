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
                      const bool domain_trace = false,
                      const double completion_fitness = 0.0,
                      const std::optional<const unsigned> seed = std::nullopt) :
        Domain<G, double>(domain_trace, completion_fitness, seed),
        _a(a),
        _b(b),
        _c(c) {}

    QuadraticFunction(const JSON& json) :
        Domain<G, double>(json),
        _a(json.at({"a"})),
        _b(json.at({"b"})),
        _c(json.at({"c"})) {}

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override
    {

        const VectorPhenotypeSpec* real_vec_pheno_spec =
            dynamic_cast<const VectorPhenotypeSpec*>(&pheno_spec);

        if(real_vec_pheno_spec == nullptr)
        {
            std::cerr << "Only vector phenotype specifications are allowed " <<
                        "with the quadratic domain!" << std::endl;
            return false;
        }

        if(real_vec_pheno_spec->get_num_params() != 1)
        {
            std::cerr << "The number of params needs to equal 1 for a 1-dimensional" <<
                " function!" << std::endl;
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
    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override {}
    void trial_reset(const unsigned trial_num) override {}

    QuadraticFunction* clone_impl() const override
    {
        return new QuadraticFunction(*this);
    }

    const double _a;
    const double _b;
    const double _c;

};

static Factory<Domain<double, double>>::Registrar quad_registrar("QuadraticFunction",
    [](const JSON& json)
    {return std::make_shared<QuadraticFunction<double>>(json);});

} // namespace NeuroEvo

#endif
