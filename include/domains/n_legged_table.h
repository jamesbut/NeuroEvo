#ifndef _N_LEGGED_TABLE_H_
#define _N_LEGGED_TABLE_H_

/*
    The n-legged table domain is a task where the aim
    is to optimise the height of a table with n legs
    whilst keeping all the legs similar size.
    This is a useful domain for testing evolvability.
*/

#include <domains/domain.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>

namespace NeuroEvo {

template <typename G>
class NLeggedTable : public Domain<G, double> {

public:

    NLeggedTable(const unsigned num_legs, const bool domain_trace = false) :
        Domain<G, double>(domain_trace, 100.),
        _num_legs(num_legs) {}

    NLeggedTable(const NLeggedTable& n_legged_table) :
        Domain<G, double>(n_legged_table._domain_trace, n_legged_table._completion_fitness),
        _num_legs(n_legged_table._num_legs) {}

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) override 
    {

        const VectorPhenotypeSpec* real_vec_pheno_spec = 
            dynamic_cast<const VectorPhenotypeSpec*>(&pheno_spec);

        if(real_vec_pheno_spec == nullptr) {

            std::cerr << "Only real vector phenotype specifications are allowed" <<
                         "with the n-legged table domain!" << std::endl;

            return false;

        }

        if(real_vec_pheno_spec->get_num_params() != _num_legs) {

            std::cerr << "The number of genes needs to equal the number of legs" << std::endl;

            return false;

        }

        return true;

    }

protected:

    void render() override {}
    void reset_domain() override {}

    NLeggedTable<G>* clone_impl() const override
    {
        return new NLeggedTable<G>(*this);
    }

private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override 
    {

        const std::vector<double> inputs = std::vector<double>();
        const std::vector<double> leg_sizes = org.get_phenotype().activate(inputs);


        //Calculate mean and std deviation of leg_sizes
        const double mean = std::accumulate(leg_sizes.begin(), leg_sizes.end(), 0.0) /
                                            leg_sizes.size();
        std::vector<double> diff(leg_sizes.size());
        std::transform(leg_sizes.begin(), leg_sizes.end(), diff.begin(),
                       [mean](double x) { return x - mean; });
        const double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        const double std_dev = std::sqrt(sq_sum / leg_sizes.size());

        //double fitness = -std_dev;
        //const double fitness = -std_dev - fabs(mean/10);
        const double fitness = -std_dev + (mean/10);


        if(this->_domain_trace) {
            std::cout << "Genotype:" << std::endl;
            std::cout << org.get_genotype() << std::endl;
            std::cout << "Phenotype:" << std::endl;
            org.get_phenotype().print_params();
            std::cout << "Mean: " << mean << std::endl;
            std::cout << "S.D.: " << std_dev << std::endl;
        }

        return fitness;

    }

    const unsigned _num_legs;

};

} // namespace NeuroEvo

#endif
