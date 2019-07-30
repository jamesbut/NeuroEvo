#include <domains/sequence_classification.h>
#include <phenotype/phenotype_specs/network_spec.h>
#include <algorithm>
#include <thread>

namespace NeuroEvo {
namespace Domains {

SequenceClassification::SequenceClassification(const unsigned DEPTH, const unsigned ZEROS_UPPER,
                                               const unsigned ZEROS_LOWER, const bool DOMAIN_TRACE,
                                               const double COMPLETION_FITNESS) :
    _DEPTH(DEPTH),
    _ZEROS_UPPER(ZEROS_UPPER),
    _ZEROS_LOWER(ZEROS_LOWER),
    _INPUT_VALUES(std::vector<int>{1, -1}),
    _rng((std::random_device())()),
    _zeros_dist(ZEROS_LOWER, ZEROS_UPPER),
    Domain(DOMAIN_TRACE, COMPLETION_FITNESS) {}

double SequenceClassification::single_run(Organism& org, unsigned rand_seed) {

    //Generate sequences of inputs
    std::vector<std::vector<int> > sequence_inputs = generate_sequences(rand_seed);

    if(_DOMAIN_TRACE) {
        for(const auto& vec : sequence_inputs) {
            for(const auto& num : vec)
                std::cout << num << " ";
            std::cout << std::endl;
        }
    }

    unsigned num_sequences_correct = 0;
    unsigned num_correct = 0;

    //Input sequences into network
    for(std::size_t i = 0; i < sequence_inputs.size(); i++) {

        //Keeps track of input sum in order to calcualte reward
        int sum_so_far = 0;
        unsigned num_correct_this_sequence = 0;

        for(std::size_t j = 0; j < sequence_inputs.at(i).size(); j++) {

            std::vector<double> inputs{(double)sequence_inputs.at(i).at(j)};
            std::vector<double> output = org.get_phenotype().activate(inputs);
            sum_so_far += sequence_inputs.at(i).at(j);

            if(sequence_inputs.at(i).at(j) != 0)
                if((sum_so_far >= 0 && output.at(0) >= 0.5) || (sum_so_far < 0 && output.at(0) < 0.5))
                    num_correct_this_sequence += 1;

            if(_DOMAIN_TRACE) {
                std::cout << "Input: " << inputs.at(0) << std::endl;
                std::cout << "Output: " << output.at(0) << std::endl;
                std::cout << "Sum so far: " << sum_so_far << std::endl;
                std::cout << std::endl;
                //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

        }

        if(num_correct_this_sequence == _DEPTH) {
            num_sequences_correct += 1;
            if(_DOMAIN_TRACE) std::cout << "CORRECT!" << std::endl;
        } else
            if(_DOMAIN_TRACE) std::cout << "INCORRECT..." << std::endl;

        if(_DOMAIN_TRACE) std::cout << "---------" <<std::endl;

        //Reset the network
        org.get_phenotype().reset();
        num_correct += num_correct_this_sequence;

    }

    if(_DOMAIN_TRACE)
        std::cout << "Num sequences correct: " << num_sequences_correct << "/"
        << sequence_inputs.size() <<std::endl;

    return (double)num_correct / (double)(sequence_inputs.size() * _DEPTH);

    //This might be too sparse a reward to start with
    //return (double)num_sequences_correct / (double)sequence_inputs.size();

}

std::vector<std::vector<int> > SequenceClassification::generate_sequences(unsigned rand_seed) {

    //I think the orginal paper does not get all permutations
    //I think it only generates a random permutation
    //Get all permutations of input sequences
    std::vector<int> empty_vec;
    std::vector<std::vector<int> > permutations_matrix;
    get_all_permutations(_INPUT_VALUES, empty_vec, _DEPTH, permutations_matrix);

    std::vector<std::vector<int> > sequences(permutations_matrix.size(), std::vector<int>());

    for(std::size_t i = 0; i < permutations_matrix.size(); i++) {

        //Add first element from permutations matrix - will be 1 or -1, no 0 first
        sequences.at(i).push_back(permutations_matrix.at(i).at(0));

        for(std::size_t j = 1; j < permutations_matrix.at(i).size(); j++) {

            const unsigned num_zeros = _zeros_dist(_rng);
            for(unsigned k = 0; k < num_zeros; k++) sequences.at(i).push_back(0);
            sequences.at(i).push_back(permutations_matrix.at(i).at(j));

        }

    }

    return sequences;

}

void SequenceClassification::get_all_permutations(const std::vector<int> VALUES,
                                                  std::vector<int> prev_values,
                                                  int length,
                                                  std::vector<std::vector<int> > &matrix_ref) {

    for(std::vector<int>::const_iterator it = VALUES.begin(); it != VALUES.end(); it++) {

        std::vector<int> new_vec;

        new_vec.insert(new_vec.end(), prev_values.begin(), prev_values.end());
        new_vec.insert(new_vec.end(), it, it+1);

        if(length == 1)
            matrix_ref.push_back(new_vec);
        else
            get_all_permutations(VALUES, new_vec, length-1, matrix_ref);

    }

}

bool SequenceClassification::check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) {

    Phenotypes::NetworkSpec* network_spec = dynamic_cast<Phenotypes::NetworkSpec*>(&pheno_spec);

    //If it is not a network
    if(network_spec == nullptr) {

        std::cout << "Only network specifications are allowed with" <<
                     " the sequence classification domain!" << std::endl;
        return false;

    }

    //Check for 1 output
    if(network_spec->NUM_OUTPUTS != 1) {

        std::cerr << "Number of outputs must be 1 for the sequence classification domain" << std::endl;
        return false;

    }

    //Check inputs
    if(network_spec->NUM_INPUTS != 1) {

        std::cerr << "Number of inputs must be 1 for the sequence classification domain" << std::endl;
        return false;

    }

    return true;

}

} // namespace Domains
} // namespace NeuroEvo
