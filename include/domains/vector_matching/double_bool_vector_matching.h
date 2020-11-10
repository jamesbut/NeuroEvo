#ifndef _DOUBLE_BOOL_VECTOR_MATCHING_H_
#define _DOUBLE_BOOL_VECTOR_MATCHING_H_

/*
    For this vector matching, the phenotype output is double and the vector creation
    policy should be one in which only ones or zeros are generated - but this is not 
    enforced because they are booleans. With this vector matching, the pheno output
    is cast to a boolean vector and then compared with the matching vector.
    This is so the fitness is calculated with relation to the double difference between
    the guess and the matching vector but a winner is counted just if the casting is
    completely correct.
*/

#include <domains/vector_matching/vector_matching.h>

namespace NeuroEvo {

template <typename G>
class DoubleBoolVectorMatching : public VectorMatching<G, double>
{

public:

    DoubleBoolVectorMatching(const std::shared_ptr<VectorCreationPolicy<double>>
                                 vector_creation_policy,
                             const bool domain_trace = false,
                             const double completion_fitness = -1e-3) :
        VectorMatching<G, double>(vector_creation_policy, domain_trace, completion_fitness) 
    {
        _bool_matching_vector = cast_vec_to_bool(this->_matching_vector);
    }

private:
    
    //Returns the negative of the distance between the phenotype vector and the match vector
    double calculate_match_value(Organism<G, double>& org) const override
    {

        //Before the fitness is calculated and returned, first cast the phenotype
        //output to a boolean vector and see whether it is equal to the matching vector
        determine_winner(org);

        return calculate_fitness(org); 
    }

    double calculate_fitness(Organism<G, double>& org) const
    {

        const std::vector<double> phenotype_vector = 
            org.get_phenotype().activate(std::vector<double>());

        double total_distance = 0.;

        for(std::size_t i = 0; i < phenotype_vector.size(); i++)
        {
            if(this->_domain_trace)
                std::cout << this->_matching_vector[i] << " " << phenotype_vector[i] 
                    << std::endl;

            total_distance += std::abs(this->_matching_vector[i] - phenotype_vector[i]);

        }
        
        //Normalised by dimension size
        //So this is average distance per trait
        return -total_distance / (double)this->_matching_vector.size();

    }

    void determine_winner(Organism<G, double>& org) const
    {
        const std::vector<double>& phenotype_vector =
            org.get_phenotype().activate(std::vector<double>());
        const std::vector<bool> org_bool_vec(cast_vec_to_bool(phenotype_vector));

        std::cout << "Org vec: " << std::endl;
        for(const auto org_vec : org_bool_vec)
            std::cout << org_vec << " ";
        std::cout << std::endl;

        std::cout << "Matching vector: " << std::endl;
        for(const auto bm : _bool_matching_vector)
            std::cout << bm << " ";
        std::cout << std::endl;

        std::cout << "Double matching vector: " << std::endl;
        for(const auto m : this->_matching_vector)
            std::cout << m << " ";
        std::cout << std::endl;

        const bool vectors_equal = std::equal(org_bool_vec.begin(), org_bool_vec.end(),
                                              _bool_matching_vector.begin());
        if(vectors_equal)
            org.set_domain_winner(true);
    }

    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override 
    {
        //Locks this reset when running in parallel
        //There were problems with the distribution without this lock
        mtx.lock();

        //Reset matching vector if it was generated from a distribution
        if(this->_vector_creation_policy)
        {
            if(this->_seed.has_value())
                this->_vector_creation_policy->seed(run_seed);
            this->_matching_vector = this->_vector_creation_policy->generate_vector(run_num);
            _bool_matching_vector = cast_vec_to_bool(this->_matching_vector);
        }

        this->print_matching_vector();

        mtx.unlock();
    }

    std::vector<bool> cast_vec_to_bool(const std::vector<double> vec) const
    {
        std::vector<bool> bool_vec(vec.size());
        for(unsigned i = 0; i < bool_vec.size(); i++)
            if(vec[i] > 0.5)
                bool_vec[i] = true;
            else
                bool_vec[i] = false;

        return bool_vec;
        
    }

    DoubleBoolVectorMatching* clone_impl() const override
    {
        return new DoubleBoolVectorMatching(*this);
    }

    std::vector<bool> _bool_matching_vector;

};

} // namespace NeuroEvo

#endif
