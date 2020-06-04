#ifndef _VECTOR_MATCHING_H_
#define _VECTOR_MATCHING_H_

#include <domains/domain.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>
#include <util/random/distribution.h>

namespace NeuroEvo {

template <typename G, typename T>
class VectorMatching : public Domain<G, T>
{

public: 

    VectorMatching(const std::vector<T>& matching_vector, 
                   const bool domain_trace = false, 
                   const double completion_fitness = 1.0) :
        _matching_vector(matching_vector),
        Domain<G, T>(domain_trace, completion_fitness) {}

    //If a matching vector is not given then one is randomly generated
    //according to a distribution
    VectorMatching(const unsigned matching_vector_size,
                   const std::unique_ptr<Distribution<T>>& matching_vector_distr,
                   const bool domain_trace = false, 
                   const double completion_fitness = 1.0) :
        _matching_vector(randomly_generate_matching_vector(matching_vector_size, 
                                                           matching_vector_distr)),
        Domain<G, T>(domain_trace, completion_fitness) {}

    bool check_phenotype_spec(PhenotypeSpec<G, T>& pheno_spec) override
    {
        VectorPhenotypeSpec<T>* vec_pheno_spec =
            dynamic_cast<VectorPhenotypeSpec<T>*>(&pheno_spec);

        if(vec_pheno_spec == nullptr)
        {
            std::cerr << "Only vector phenotype specifications are allowed" <<
                " for the Vector Matching domain!" << std::endl;
            return false;
        }

        if(vec_pheno_spec->get_num_params() != _matching_vector.size())
        {
            std::cerr << "The number of phenotype parameters does not match the size of the" <<
                " matching vector" << std::endl;
            return false;
        }

        return true;
    }

protected:

    void render() override {}

private: 

    double single_run(Organism<G, T>& org, unsigned rand_seed) override
    {
        const std::vector<T> phenotype_vector = 
            org.get_phenotype().activate(std::vector<double>());

        const double fitness = calculate_match_value(phenotype_vector);

        return fitness;
    }

    //Calculates how closely the phenotype vector matches the matching vector
    //Returns a value between 0 and 1 with 1 being a perfect match
    double calculate_match_value(const std::vector<T>& phenotype_vector) const 
    {
        unsigned num_matches = 0;

        for(std::size_t i = 0; i < phenotype_vector.size(); i++)
        {
            if(this->_domain_trace)
                std::cout << _matching_vector[i] << " " << phenotype_vector[i] << " ";

            //This comparison might not work so well for doubles
            if(_matching_vector[i] == phenotype_vector[i])
            {
                num_matches += 1;
                if(this->_domain_trace) std::cout << "Match!" << std::endl;

            } else
                if(this->_domain_trace) std::cout << "No match" << std::endl;

        }

        const double match_value = (double)num_matches / (double)phenotype_vector.size(); 

        if(this->_domain_trace) 
        {
            std::cout << "Num matches: " << num_matches << std::endl;
            std::cout << "Match value: " << match_value << std::endl;
        }

        return match_value;
    }

    const std::vector<T> randomly_generate_matching_vector(
        const unsigned matching_vector_size,
        const std::unique_ptr<Distribution<T>>& distr) const
    {
        std::vector<T> matching_vector;
        matching_vector.reserve(matching_vector_size);

        for(unsigned i = 0; i < matching_vector_size; i++)   
            matching_vector.push_back(distr->next());

        return matching_vector;
    }

    std::vector<T> _matching_vector;

};

} // namespace NeuroEvo

#endif
