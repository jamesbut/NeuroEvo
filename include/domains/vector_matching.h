#ifndef _VECTOR_MATCHING_H_
#define _VECTOR_MATCHING_H_

#include <domains/domain.h>

namespace NeuroEvo {

template <typename G, typename T>
class VectorMatching : public Domain<G, T>
{

public: 

    VectorMatching(const std::vector<T>& matching_vector, 
                   const bool domain_trace = false, 
                   const double completion_fitness = 0.0) :
        _matching_vector(matching_vector),
        Domain<G, T>(domain_trace, completion_fitness) {}

    bool check_phenotype_spec(PhenotypeSpec<G, T>& pheno_spec) override
    {
        return true;
    }

protected:

    void render() override {}

private: 

    double single_run(Organism<G, T>& org, unsigned rand_seed) override
    {
        std::vector<T> phenotype_vector = org.get_phenotype().activate(std::vector<double>());

        const double fitness = 0.;

        return fitness;
    }

    //Returns a value between 0 and 1 with 1 being a perfect match
    double calculate_match_value(const std::vector<T>& phenotype_vector) 
    {

    }

    std::vector<T> _matching_vector;

};

} // namespace NeuroEvo

#endif
