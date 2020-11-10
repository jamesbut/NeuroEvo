#ifndef _VECTOR_MATCHING_H_
#define _VECTOR_MATCHING_H_

#include <domains/domain.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>
#include <util/statistics/distributions/distribution.h>
#include <util/vector_creation/vector_creation_policy.h>

#include <mutex>

namespace NeuroEvo {

std::mutex mtx;

template <typename G, typename T>
class VectorMatching : public Domain<G, T>
{

public: 

    VectorMatching(const std::vector<T>& matching_vector, 
                   const bool domain_trace, 
                   const double completion_fitness) :
        Domain<G, T>(domain_trace, completion_fitness),
        _matching_vector(matching_vector),
        _vector_creation_policy(nullptr) {}

    //If a matching vector is not given then one is randomly generated
    //according to a distribution
    VectorMatching(const std::shared_ptr<VectorCreationPolicy<T>> vector_creation_policy,
                   const bool domain_trace, 
                   const double completion_fitness) :
        Domain<G, T>(domain_trace, completion_fitness),
        _matching_vector(vector_creation_policy->generate_vector(0)),
        _vector_creation_policy(vector_creation_policy)
    {
        //print_matching_vector();
    }

    VectorMatching(const VectorMatching<G, T>& vector_matching) :
        Domain<G, T>(vector_matching._domain_trace, vector_matching._completion_fitness),
        _matching_vector(vector_matching._matching_vector),
        _vector_creation_policy(vector_matching._vector_creation_policy) {}

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override
    {
         auto vec_pheno_spec = dynamic_cast<const VectorPhenotypeSpec*>(&pheno_spec);

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

    //Calculates how closely the phenotype vector matches the matching vector
    //This is calculated differently depending on the trait type
    virtual double calculate_match_value(Organism<G, T>& org) const = 0;
    void render() override {}

    void print_matching_vector() const
    {
        std::cout << "Matching vector: "; 
        for(const auto v : this->_matching_vector)
            std::cout << v << " ";
        std::cout << std::endl;
    }

    std::vector<T> _matching_vector;
    std::shared_ptr<VectorCreationPolicy<T>> _vector_creation_policy;

private: 

    double single_run(Organism<G, T>& org, unsigned rand_seed) override
    {
        const std::vector<T> phenotype_vector = 
            org.get_phenotype().activate(std::vector<double>());

        const double fitness = calculate_match_value(org);

        return fitness;
    }

    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override 
    {
        //Locks this reset when running in parallel
        //There were problems with the distribution without this lock
        mtx.lock();

        //Reset matching vector if it was generated from a distribution
        if(_vector_creation_policy)
            _matching_vector = _vector_creation_policy->generate_vector(run_num);
        print_matching_vector();

        mtx.unlock();
    }
    void trial_reset(const unsigned trial_num) override {}

};

} // namespace NeuroEvo

#endif
