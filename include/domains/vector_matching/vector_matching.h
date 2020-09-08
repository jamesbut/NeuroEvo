#ifndef _VECTOR_MATCHING_H_
#define _VECTOR_MATCHING_H_

#include <domains/domain.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>
#include <util/statistics/distributions/distribution.h>

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
        _matching_vector(matching_vector) {}

    //If a matching vector is not given then one is randomly generated
    //according to a distribution
    VectorMatching(const unsigned matching_vector_size,
                   //Distribution<T>* matching_vector_distr,
                   const std::shared_ptr<Distribution<T>> matching_vector_distr,
                   const bool symmetric_match_vector,
                   const bool domain_trace, 
                   const double completion_fitness) :
        Domain<G, T>(domain_trace, completion_fitness),
        _matching_vector_distr(matching_vector_distr),
        _symmetric_match_vector(symmetric_match_vector),
        _matching_vector(randomly_generate_matching_vector(matching_vector_size)) 
    {
        print_matching_vector();
    }

    VectorMatching(const VectorMatching<G, T>& vector_matching) :
        Domain<G, T>(vector_matching._domain_trace, vector_matching._completion_fitness),
        _matching_vector_distr(vector_matching._matching_vector_distr),
        _symmetric_match_vector(vector_matching._symmetric_match_vector),
        _matching_vector(vector_matching._matching_vector) {}


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
    virtual double calculate_match_value(const std::vector<T>& phenotype_vector) const = 0;
    void render() override {}

private: 

    double single_run(Organism<G, T>& org, unsigned rand_seed) override
    {
        const std::vector<T> phenotype_vector = 
            org.get_phenotype().activate(std::vector<double>());

        const double fitness = calculate_match_value(phenotype_vector);

        return fitness;
    }

    const std::vector<T> randomly_generate_matching_vector(
        const std::size_t matching_vector_size) const
    {

        std::vector<T> matching_vector;
        matching_vector.reserve(matching_vector_size);

        if(_symmetric_match_vector) 
        {
            //If symmetric match vector, check whether matching vector size is even.
            //If not exit - I don't think this is how I should handle the error but
            //maybe I can come back to this later.
            if(matching_vector_size % 2 != 0)
            {
                std::cerr << "Cannot have a symmetric match vector of odd size!" << std::endl;
                exit(0);
            }

            //Randomly generate vector of half the size, duplicate, reverse and concatenate
            const unsigned num_unique_elements = matching_vector_size/2;
            std::vector<T> first_half_matching_vector;
            first_half_matching_vector.reserve(num_unique_elements);

            for(unsigned i = 0; i < num_unique_elements; i++)   
                first_half_matching_vector.push_back(_matching_vector_distr->next());

            //Create symmetric second half
            std::vector<T> second_half_matching_vector(first_half_matching_vector);
            std::reverse(second_half_matching_vector.begin(), second_half_matching_vector.end());

            //Concatenate
            first_half_matching_vector.insert(first_half_matching_vector.begin(),
                                              second_half_matching_vector.begin(),
                                              second_half_matching_vector.end());
            matching_vector = first_half_matching_vector;

        } else
            for(unsigned i = 0; i < matching_vector_size; i++)   
                matching_vector.push_back(_matching_vector_distr->next());

        return matching_vector;
    }

    void reset_domain() override 
    {
        //Locks this reset when running in parallel
        //There were problems with the distribution without this lock
        mtx.lock();

        //Reset matching vector if it was generated from a distribution
        if(_matching_vector_distr)
            _matching_vector = randomly_generate_matching_vector(_matching_vector.size());
        print_matching_vector();

        mtx.unlock();
    }

    void print_matching_vector() const
    {
        std::cout << "Matching vector: "; 
        for(const auto& v : this->_matching_vector)
            std::cout << v << " ";
        std::cout << std::endl;
    }

    const std::shared_ptr<Distribution<T>> _matching_vector_distr;
    const bool _symmetric_match_vector;

protected:

    std::vector<T> _matching_vector;

};

} // namespace NeuroEvo

#endif
