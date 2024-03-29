#ifndef _SUPERVISED_FEEDFORWARD_H_
#define _SUPERVISED_FEEDFORWARD_H_

/*
    This model is a feedforward network trained in a supervised way.
*/

#include <util/models/trainable_model.h>

namespace NeuroEvo {

class SupervisedFeedForward : public TrainableModel
{

public:

    SupervisedFeedForward(
        NetworkBuilder net_builder,
        const torch::Tensor& training_labels,
        const std::optional<const torch::Tensor>& test_labels = std::nullopt
    );

    SupervisedFeedForward(const JSON& config);

    bool train(const unsigned num_epochs,
               const unsigned batch_size,
               const torch::Tensor& training_data,
               const std::optional<const torch::Tensor>& test_data = std::nullopt,
               const double weight_decay = 0.,
               const bool trace = false,
               const unsigned test_every = 1e6) override;

private:

    torch::Tensor loss_function(const torch::Tensor& output,
                                const torch::Tensor& labels) const;

    NetworkBuilder create_feedforward_builder(const JSON& config) const;

    const torch::Tensor read_training_labels(const JSON& config) const;
    std::optional<const torch::Tensor> read_test_labels(const JSON& config) const;

    const torch::Tensor _training_labels;
    const std::optional<const torch::Tensor> _test_labels;

};

} // namespace NeuroEvo

#endif
