#ifndef _BOOL_VECTOR_MATCHING_
#define _BOOL_VECTOR_MATCHING_

#include <domains/vector_matching/vector_matching.h>

namespace NeuroEvo {

template<typename G>
class BoolVectorMatching : public VectorMatching<G, bool>
{

public:

    BoolVectorMatching(const std::vector<bool>& matching_vector,
                       const bool domain_trace = false,
                       const double completion_fitness = 1.) :
        VectorMatching<G, bool>(matching_vector, domain_trace, completion_fitness) {}

    //If a matching vector is not given then one is randomly generated
    //according to a distribution
    BoolVectorMatching(const unsigned matching_vector_size,
                       const std::shared_ptr<Distribution<bool>> matching_vector_distr,
                       const bool symmetric_match_vector = false,
                       const bool domain_trace = false,
                       const double completion_fitness = 1.) :
        VectorMatching<G, bool>(matching_vector_size, matching_vector_distr, 
                                symmetric_match_vector, domain_trace, completion_fitness) {}

private:

    //Returns a value between 0 and 1 with 1 being a perfect match
    double calculate_match_value(const std::vector<bool>& phenotype_vector) const override
    {
        unsigned num_matches = 0;

        for(std::size_t i = 0; i < phenotype_vector.size(); i++)
        {
            if(this->_domain_trace)
                std::cout << this->_matching_vector[i] << " " << phenotype_vector[i] << " ";

            if(this->_matching_vector[i] == phenotype_vector[i])
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

    void exp_run_reset_impl(const unsigned run_num) override 
    {
        /*
        std::vector<bool> matching_vector(this->_matching_vector.size(), false);
        matching_vector.at(run_num) = true;
        this->_matching_vector = matching_vector;
        */
        
        //Locks this reset when running in parallel
        //There were problems with the distribution without this lock
        mtx.lock();

        //Reset matching vector if it was generated from a distribution
        if(this->_matching_vector_distr)
            this->_matching_vector = this->randomly_generate_matching_vector(
                    this->_matching_vector.size());

        mtx.unlock();
    }

    BoolVectorMatching* clone_impl() const override
    {
        return new BoolVectorMatching(*this);
    }

};

} // namespace NeuroEvo

#endif
