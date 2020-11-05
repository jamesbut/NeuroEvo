#ifndef _DOUBLE_VECTOR_MATCHING_H_
#define _DOUBLE_VECTOR_MATCHING_H_

#include <domains/vector_matching/vector_matching.h>

namespace NeuroEvo {

template<typename G>
class DoubleVectorMatching : public VectorMatching<G, double>
{

public:

    DoubleVectorMatching(const std::vector<double>& matching_vector,
                         const bool domain_trace = false,
                         const double completion_fitness = -1e-3) :
        VectorMatching<G, double>(matching_vector, domain_trace, completion_fitness) {}

    DoubleVectorMatching(const std::shared_ptr<VectorCreationPolicy<double>> 
                             vector_creation_policy,
                         const bool domain_trace = false,
                         const double completion_fitness = -1e-3) :
        VectorMatching<G, double>(vector_creation_policy, domain_trace, completion_fitness) {}

protected:

    //Returns the negative of the distance between the phenotype vector and the match vector
    double calculate_match_value(const std::vector<double>& phenotype_vector) const override
    {
        double total_distance = 0.;

        for(std::size_t i = 0; i < phenotype_vector.size(); i++)
        {
            if(this->_domain_trace)
                std::cout << this->_matching_vector[i] << " " << phenotype_vector[i] 
                    << std::endl;

            total_distance += std::abs(this->_matching_vector[i] - phenotype_vector[i]);

        }

        //return -total_distance;
        
        //Normalised by dimension size
        //So this is average distance per trait
        return -total_distance / (double)this->_matching_vector.size();
        
    }

private:

    DoubleVectorMatching<G>* clone_impl() const override
    {
        return new DoubleVectorMatching<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
