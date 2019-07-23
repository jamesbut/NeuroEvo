#include <domains/n_legged_table.h>
#include <phenotype/phenotype_specs/real_vector_phenotype_spec.h>
#include <algorithm>
#include <cmath>

NLeggedTable::NLeggedTable(const unsigned NUM_LEGS, const bool DOMAIN_TRACE) :
    _NUM_LEGS(NUM_LEGS),
    Domain(DOMAIN_TRACE, 100) {}

double NLeggedTable::single_run(Organism& org, unsigned rand_seed) {

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


    if(_DOMAIN_TRACE) {
        std::cout << "Genotype:" << std::endl;
        org.get_genotype().print_genotype();
        std::cout << "Phenotype:" << std::endl;
        org.get_phenotype().print_params();
        std::cout << "Mean: " << mean << std::endl;
        std::cout << "S.D.: " << std_dev << std::endl;
    }

    return fitness;

}

bool NLeggedTable::check_phenotype_spec(PhenotypeSpec& pheno_spec) {

    RealVectorPhenotypeSpec* real_vec_pheno_spec;
    real_vec_pheno_spec = dynamic_cast<RealVectorPhenotypeSpec*>(&pheno_spec);

    if(real_vec_pheno_spec == nullptr) {

        std::cerr << "Only real vector phenotype specifications are allowed" <<
                     "with the n-legged table domain!" << std::endl;

        return false;

    }

    if(real_vec_pheno_spec->NUM_GENES != _NUM_LEGS) {

        std::cerr << "The number of genes needs to equal the number of legs" << std::endl;

        return false;

    }

    return true;

}
