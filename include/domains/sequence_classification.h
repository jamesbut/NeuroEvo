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

template <typename G>
class SequenceClassification : public Domain<G, double>
{

public:

    SequenceClassification(const unsigned depth, const unsigned zeros_upper,
                           const unsigned zeros_lower, const bool domain_trace = false,
                           const double completion_fitness = 1) :
        _depth(depth),
        _zeros_upper(zeros_upper),
        _zeros_lower(zeros_lower),
        _input_values(std::vector<int>{1, -1}),
        _rng((std::random_device())()),
        _zeros_dist(zeros_lower, zeros_upper),
        Domain<G, double>(domain_trace, completion_fitness) {}

    bool check_phenotype_spec(PhenotypeSpec<G, double>& pheno_spec) override 
    {

        NetworkBuilder* network_builder = dynamic_cast<NetworkBuilder*>(&pheno_spec);

        //If it is not a network
        if(network_builder == nullptr) 
        {
            std::cout << "Only network specifications are allowed with" <<
                        " the sequence classification domain!" << std::endl;
            return false;
        }

        //Check for 1 output
        if(network_builder->get_num_outputs() != 1) 
        {
            std::cerr << "Number of outputs must be 1 for the sequence classification domain" << 
                std::endl;
            return false;
        }

        //Check inputs
        if(network_builder->get_num_inputs() != 1) 
        {
            std::cerr << "Number of inputs must be 1 for the sequence classification domain" << 
                std::endl;
            return false;
        }

        return true;

    }

protected:

    void render() override {}

private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override 
    {

        //Generate sequences of inputs
        std::vector<std::vector<int> > sequence_inputs = generate_sequences(rand_seed);

        if(this->_domain_trace) 
        {
            for(const auto& vec : sequence_inputs) 
            {
                for(const auto& num : vec)
                    std::cout << num << " ";
                std::cout << std::endl;
            }
        }

        unsigned num_sequences_correct = 0;
        unsigned num_correct = 0;

        //Input sequences into network
        for(std::size_t i = 0; i < sequence_inputs.size(); i++) 
        {

            //Keeps track of input sum in order to calcualte reward
            int sum_so_far = 0;
            unsigned num_correct_this_sequence = 0;

            for(std::size_t j = 0; j < sequence_inputs.at(i).size(); j++) 
            {

                std::vector<double> inputs{(double)sequence_inputs.at(i).at(j)};
                std::vector<double> output = org.get_phenotype().activate(inputs);
                sum_so_far += sequence_inputs.at(i).at(j);

                if(sequence_inputs.at(i).at(j) != 0)
                    if((sum_so_far >= 0 && output.at(0) >= 0.5) || 
                       (sum_so_far < 0 && output.at(0) < 0.5))
                        num_correct_this_sequence += 1;

                if(this->_domain_trace) 
                {
                    std::cout << "Input: " << inputs.at(0) << std::endl;
                    std::cout << "Output: " << output.at(0) << std::endl;
                    std::cout << "Sum so far: " << sum_so_far << std::endl;
                    std::cout << std::endl;
                    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

            }

            if(num_correct_this_sequence == _depth) 
            {
                num_sequences_correct += 1;
                if(this->_domain_trace) std::cout << "correct!" << std::endl;
            } else
                if(this->_domain_trace) std::cout << "incorrect..." << std::endl;

            if(this->_domain_trace) std::cout << "---------" <<std::endl;

            //Reset the network
            org.get_phenotype().reset();
            num_correct += num_correct_this_sequence;

        }

        if(this->_domain_trace)
            std::cout << "Num sequences correct: " << num_sequences_correct << "/"
            << sequence_inputs.size() <<std::endl;

        return (double)num_correct / (double)(sequence_inputs.size() * _depth);

        //This might be too sparse a reward to start with
        //return (double)num_sequences_correct / (double)sequence_inputs.size();

    }

    std::vector<std::vector<int> > generate_sequences(unsigned rand_seed) 
    {

        //I think the orginal paper does not get all permutations
        //I think it only generates a random permutation
        //Get all permutations of input sequences
        std::vector<int> empty_vec;
        std::vector<std::vector<int> > permutations_matrix;
        get_all_permutations(_input_values, empty_vec, _depth, permutations_matrix);

        std::vector<std::vector<int> > sequences(permutations_matrix.size(), std::vector<int>());

        for(std::size_t i = 0; i < permutations_matrix.size(); i++) 
        {

            //Add first element from permutations matrix - will be 1 or -1, no 0 first
            sequences.at(i).push_back(permutations_matrix.at(i).at(0));

            for(std::size_t j = 1; j < permutations_matrix.at(i).size(); j++) 
            {
                const unsigned num_zeros = _zeros_dist(_rng);
                for(unsigned k = 0; k < num_zeros; k++) sequences.at(i).push_back(0);
                sequences.at(i).push_back(permutations_matrix.at(i).at(j));
            }

        }

        return sequences;

    }

    void get_all_permutations(const std::vector<int> values,
                              std::vector<int> prev_values,
                              int length,
                              std::vector<std::vector<int> > &matrix_ref) 
    {

        for(std::vector<int>::const_iterator it = values.begin(); it != values.end(); it++) 
        {
            std::vector<int> new_vec;

            new_vec.insert(new_vec.end(), prev_values.begin(), prev_values.end());
            new_vec.insert(new_vec.end(), it, it+1);

            if(length == 1)
                matrix_ref.push_back(new_vec);
            else
                get_all_permutations(values, new_vec, length-1, matrix_ref);
        }

    }

    const unsigned _depth;
    const unsigned _zeros_lower;
    const unsigned _zeros_upper;

    const std::vector<int> _input_values;

    //Random number generators
    std::mt19937 _rng;
    std::uniform_int_distribution<int> _zeros_dist;

};

} // namespace NeuroEvo

#endif
