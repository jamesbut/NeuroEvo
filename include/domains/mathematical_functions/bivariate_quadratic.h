#ifndef _BIVARIATE_QUADRATIC_H_
#define _BIVARIATE_QUADRATIC_H_

/*
 * Function of the form: f(x,y) = ax^2 + by^2 + cxy + dx + ey + f
 */

#include <domains/domain.h>

namespace NeuroEvo {

template <typename G>
class BivariateQuadratic : public Domain<G, double>
{

public:

    BivariateQuadratic(const double a, const double b, const double c,
                       const double d, const double e, const double f,
                       const bool domain_trace = false,
                       const double completion_fitness = 0.0,
                       const std::optional<const unsigned> seed = std::nullopt) :
        Domain<G, double>(domain_trace, completion_fitness, seed),
        _a(a),
        _b(b),
        _c(c),
        _d(d),
        _e(e),
        _f(f) {}

    BivariateQuadratic(const JSON& json) :
        Domain<G, double>(
            json,
            json.value({"completion_fitness"}, 0.0)
        ),
        _a(json.at({"a"})),
        _b(json.at({"b"})),
        _c(json.at({"c"})),
        _d(json.at({"d"})),
        _e(json.at({"e"})),
        _f(json.at({"f"})) {}

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override
    {

        const VectorPhenotypeSpec* real_vec_pheno_spec =
            dynamic_cast<const VectorPhenotypeSpec*>(&pheno_spec);

        if(real_vec_pheno_spec == nullptr)
        {
            std::cerr << "Only vector phenotype specifications are allowed " <<
                        "with the bivariate quadratic domain!" << std::endl;
            return false;
        }

        if(real_vec_pheno_spec->get_num_params() != 2)
        {
            std::cerr << "The number of params needs to equal 2 for a "
                "2-dimensional function!" << std::endl;
            std::cerr << "Number of params given: " << 
                real_vec_pheno_spec->get_num_params() << std::endl;
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
        const double y = output.at(1);
        const double z = _a * pow(x, 2) + _b * pow(y, 2) + _c * x * y +
                         _d * x + _e * y + _f;

        return z;
    }

    void render() override {}
    void exp_run_reset_impl(const unsigned run_num,
                            const std::optional<unsigned>& run_seed) override {}
    void trial_reset(const unsigned trial_num) override {}

    JSON to_json_impl() const override
    {
        JSON json;
        json.emplace("name", "BivariateQuadratic");
        json.emplace("a", _a);
        json.emplace("b", _b);
        json.emplace("c", _c);
        json.emplace("d", _d);
        json.emplace("e", _e);
        json.emplace("f", _f);
        return json;
    }

    BivariateQuadratic* clone_impl() const override
    {
        return new BivariateQuadratic(*this);
    }

    const double _a, _b, _c, _d, _e, _f;

};

static Factory<Domain<double, double>>::Registrar 
    bivariate_quad_registrar(
        "BivariateQuadratic",
        [](const JSON& json)
        {return std::make_shared<BivariateQuadratic<double>>(json);}
    );

} // namespace NeuroEvo

#endif
