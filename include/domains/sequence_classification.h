#ifndef _SEQUENCE_CLASSIFICATION_H_
#define _SEQUENCE_CLASSIFICATION_H_

/*
    This sequence classification task consists of a
    sequence of 1s, -1s and 0s being presented to a network
    and the network must keep track of the overall value.
    This domain is good for testing memory capabilities of
    a network.
*/

#include <domains/domain.h>
#include <random>

namespace NeuroEvo {
namespace Domains {

class SequenceClassification : public Domain {

public:

    SequenceClassification(const unsigned DEPTH, const unsigned ZEROS_UPPER,
                           const unsigned ZEROS_LOWER, const bool DOMAIN_TRACE = false,
                           const double COMPLETION_FITNESS = 1);

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override;

protected:

    void render() override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

    std::vector<std::vector<int> > generate_sequences(unsigned rand_seed);
    void get_all_permutations(const std::vector<int> VALUES,
                              std::vector<int> prev_values,
                              int length,
                              std::vector<std::vector<int> > &matrix_ref);

    const unsigned _DEPTH;
    const unsigned _ZEROS_LOWER;
    const unsigned _ZEROS_UPPER;

    const std::vector<int> _INPUT_VALUES;

    //Random number generators
    std::mt19937 _rng;
    std::uniform_int_distribution<int> _zeros_dist;

};

} // namespace Domains
} // namespace NeuroEvo

#endif
