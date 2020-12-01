#ifndef _GENERATIVE_MODEL_H_
#define _GENERATIVE_MODEL_H_

/* 
 * This class defines the interface for a generative model
 */

#include <phenotype/neural_network/torch_network.h>
#include <util/models/trainable_model.h>

namespace NeuroEvo {

class GenerativeModel : public TrainableModel
{

public:

    GenerativeModel(const torch::Tensor& training_data,
                    const std::optional<const torch::Tensor>& test_data) :
        TrainableModel(training_data, test_data) {}

    virtual ~GenerativeModel() = default;

    virtual torch::Tensor generate(const torch::Tensor& code) const = 0;

    //Returns network that maps latent space to higher dimensional space:
    virtual const std::unique_ptr<TorchNetwork>& get_decoder() const = 0;

};

} // namespace NeuroEvo

#endif
