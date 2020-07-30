#ifndef _GENERATIVE_MODEL_H_
#define _GENERATIVE_MODEL_H_

/* 
 * This class defines the interface for a generative model
 */

#include <torch/types.h>

namespace NeuroEvo {

class GenerativeModel
{

public:

    GenerativeModel(const torch::Tensor& training_data,
                    const std::optional<const torch::Tensor>& test_data) :
        _training_data(training_data),
        _test_data(test_data) {}

    virtual void train(const unsigned num_epochs, const unsigned batch_size,
                       const bool trace = false, const unsigned test_every = 1e6) = 0;

    virtual torch::Tensor generate(const torch::Tensor& code) const = 0;

protected:

    const torch::Tensor _training_data;
    const std::optional<const torch::Tensor> _test_data;

};

} // namespace NeuroEvo

#endif
