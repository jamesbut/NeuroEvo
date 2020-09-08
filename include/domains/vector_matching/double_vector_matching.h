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

    DoubleVectorMatching(const unsigned matching_vector_size,
                         const std::shared_ptr<Distribution<double>> matching_vector_distr,
                         const bool symmetric_match_vector = false,
                         const bool domain_trace = false,
                         const double completion_fitness = -1e-3) :
        VectorMatching<G, double>(matching_vector_size, matching_vector_distr, 
                                  symmetric_match_vector, domain_trace, completion_fitness) {}

protected:

    //Returns the negative of the distance between the phenotype vector and the match vector
    double calculate_match_value(const std::vector<double>& phenotype_vector) const override
    {
        double total_distance = 0.;

        for(std::size_t i = 0; i < phenotype_vector.size(); i++)
        {
            if(this->_domain_trace)
                std::cout << this->_matching_vector[i] << " " << phenotype_vector[i] << std::endl;

            total_distance += std::abs(this->_matching_vector[i] - phenotype_vector[i]);

        }

        return -total_distance;
        
    }

private:

    DoubleVectorMatching<G>* clone_impl() const override
    {
        return new DoubleVectorMatching<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
