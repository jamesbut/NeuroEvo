#ifndef _TRAINABLE_MODEL_H_
#define _TRAINABLE_MODEL_H_

/*
    This class defines the interface for a trainable model
*/

#include <phenotype/neural_network/torch_network.h>
#include <boost/filesystem.hpp>
#include <phenotype/phenotype_specs/network_builder.h>
#include <util/torch_utils.h>

namespace NeuroEvo {

class TrainableModel
{

public:

    TrainableModel(const torch::Tensor& training_data,
                   const std::optional<const torch::Tensor>& test_data,
                   NetworkBuilder& model_net_builder,
                   const std::string& model_file_name) :
        _training_data(training_data),
        _test_data(test_data),
        _model(build_torch_network(model_net_builder, nullptr)),
        _model_file_name(model_file_name) {}

    virtual ~TrainableModel() = default;

    virtual void train(const unsigned num_epochs, const unsigned batch_size,
                       const double weight_decay = 0., const bool trace = false, 
                       const unsigned test_every = 1e6) = 0;
    
    //Returns model
    //In the case of the autoencoder and VAE, this is the decoder
    //In the case of the GAN, this is the generator
    const std::unique_ptr<TorchNetwork>& get_model() const
    {
        return _model;
    }

    //Performs forward pass of model
    virtual torch::Tensor forward(const torch::Tensor& input) const
    {
        std::cout << "Model input: " << std::endl;
        std::cout << input << std::endl;
        const torch::Tensor output = _model->forward(input);
        std::cout << "Model output: " << std::endl;
        std::cout << output << std::endl;
        return output;
    }

    double get_loss() const
    {
        return _loss;
    }
    
    //Write model to file
    void write_model(const std::optional<const unsigned>& epoch_num = std::nullopt) const
    {
        //Check decoders directory exists first
        if(!boost::filesystem::exists(_model_folder_path))
            boost::filesystem::create_directory(_model_folder_path);

        //Add epoch number at beginning of file name if one is given
        std::string model_file_name = _model_file_name;  
        if(epoch_num.has_value())
            model_file_name = std::to_string(epoch_num.value()) + "_" + model_file_name;
        const std::string model_file_path = _model_folder_path + model_file_name;

        _model->write(model_file_path);

    }

    static std::string generate_ie_file_path(
        const unsigned ie_type,
        const std::optional<const std::string>& prefix = std::nullopt)
    {
        std::string ie_file_name;
        switch(ie_type)
        {
            case 0:
                ie_file_name = "ie_ae.pt";
                break;
            case 1:
                ie_file_name = "ie_vae.pt";
                break;
            case 2:
                ie_file_name = "ie_gan.pt";
                break;
            case 3:
                ie_file_name = "ie_supervised.pt";
                break;
        }

        if(prefix.has_value())
            ie_file_name = prefix.value() + ie_file_name;

        return _model_folder_path + ie_file_name;
    }

    void print_params() const
    {
        std::cout << _model->parameters() << std::endl;
    }

protected:

    const torch::Tensor _training_data;
    const std::optional<const torch::Tensor> _test_data;
    double _loss;

    std::unique_ptr<TorchNetwork> _model;

private:

    const std::string _model_file_name;
    inline static const std::string _model_folder_path = 
        std::string(NEURO_EVO_CMAKE_SRC_DIR) + 
        std::string("/config/decoders/");

};

} // namespace NeuroEvo

#endif
