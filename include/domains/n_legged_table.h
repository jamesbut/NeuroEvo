#ifndef _N_LEGGED_TABLE_H_
#define _N_LEGGED_TABLE_H_

/*
    The n-legged table domain is a task where the aim
    is to optimise the height of a table with n legs
    whilst keeping all the legs similar size.
    This is a useful domain for testing evolvability.
*/

#include <domains/domain.h>

namespace NeuroEvo {

class NLeggedTable : public Domain {

public:

    NLeggedTable(const unsigned num_legs, const bool domain_trace = false);
        _num_legs(num_legs),
        Domain(domain_trace, 100) {}

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override {

        Phenotypes::RealVectorPhenotypeSpec* real_vec_pheno_spec;
        real_vec_pheno_spec = dynamic_cast<Phenotypes::RealVectorPhenotypeSpec*>(&pheno_spec);

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

private:

    double single_run(Organism& org, unsigned rand_seed) override {

        std::vector<double> inputs = std::vector<double>();
        std::vector<double> leg_sizes = org.get_phenotype().activate(inputs);


        //Calculate mean and std deviation of leg_sizes
        const double mean = std::accumulate(leg_sizes.begin(), leg_sizes.end(), 0.0) /
                            leg_sizes.size();
        std::vector<double> diff(leg_sizes.size());
        std::transform(leg_sizes.begin(), leg_sizes.end(), diff.begin(),
                    [mean](double x) { return x - mean; });
        double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        double std_dev = std::sqrt(sq_sum / leg_sizes.size());

        //double fitness = -std_dev;
        double fitness = -std_dev - fabs(mean/10);
        //double fitness = -std_dev - (mean/10);


        if(_domain_trace) {
            std::cout << "Genotype:" << std::endl;
            org.get_genotype().print_genotype();
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
