#ifndef _TRAINABLE_MODEL_H_
#define _TRAINABLE_MODEL_H_

/*
    This class defines the interface for a trainable model
*/

#include <phenotype/neural_network/torch_network.h>
#include <boost/filesystem.hpp>
#include <phenotype/phenotype_specs/network_builder.h>
#include <util/torch/batch_utils.h>

namespace NeuroEvo {

class TrainableModel
{

public:

    TrainableModel(NetworkBuilder& model_net_builder,
                   const std::string& model_file_name);

    virtual ~TrainableModel() = default;

    virtual bool train(const unsigned num_epochs, const unsigned batch_size,
                       const torch::Tensor& training_data,
                       const std::optional<const torch::Tensor>& test_data,
                       const double weight_decay = 0., const bool trace = false,
                       const unsigned test_every = 1e6) = 0;

    //Returns model
    //In the case of the autoencoder and VAE, this is the decoder
    //In the case of the GAN, this is the generator
    const std::unique_ptr<TorchNetwork>& get_model() const;
    double get_loss() const;

    //Prints model params
    void print_params() const;

    //Performs forward pass of model
    torch::Tensor forward(const torch::Tensor& input, const bool trace = false) const;

    //Write model to file
    //Can give prefix to model file name, this is typically an epoch number
    void write_model(const std::string& model_dir,
                     const std::optional<const unsigned>& prefix = std::nullopt) const;

protected:

    //Determines whether training is stuck in a local minima by checking the current
    //loss is greater than some plataeu loss when training has reached a certain
    //test_epoch
    class LocalMinChecker
    {

        public:
            LocalMinChecker(const unsigned test_epoch, const double plateau_loss);

            bool is_in_local_min(const unsigned current_epoch,
                                 const double current_loss);

        private:
            const unsigned _test_epoch;
            const unsigned _plateau_loss;

    };

    double _loss;

    std::unique_ptr<TorchNetwork> _model;

private:

    const std::string _model_file_name;

};

} // namespace NeuroEvo

#endif
