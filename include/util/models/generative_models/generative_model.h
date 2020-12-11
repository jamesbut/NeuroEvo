#ifndef _GENERATIVE_MODEL_H_
#define _GENERATIVE_MODEL_H_

/* 
 * This class defines the interface for a generative model
 */

#include <phenotype/neural_network/torch_network.h>
#include <util/models/trainable_model.h>
#include <boost/filesystem.hpp>

namespace NeuroEvo {

class GenerativeModel : public TrainableModel
{

public:

    GenerativeModel(const torch::Tensor& training_data,
                    const std::optional<const torch::Tensor>& test_data,
                    const std::string& ie_file_name) :
        TrainableModel(training_data, test_data),
        _ie_file_name(ie_file_name) {}

    virtual ~GenerativeModel() = default;

    virtual torch::Tensor generate(const torch::Tensor& code) const = 0;

    //Returns network that maps latent space to higher dimensional space:
    virtual const std::unique_ptr<TorchNetwork>& get_decoder() const = 0;

    //Write decoder to file
    void write_decoder(const std::optional<const unsigned>& epoch_num = std::nullopt) const
    {
        //Check decoders directory exists first
        if(!boost::filesystem::exists(_ie_folder_path))
            boost::filesystem::create_directory(_ie_folder_path);

        //Add epoch number at beginning of file name if one is given
        std::string ie_file_name = _ie_file_name;  
        if(epoch_num.has_value())
            ie_file_name = std::to_string(epoch_num.value()) + "_" + ie_file_name;
        const std::string ie_file_path = _ie_folder_path + ie_file_name;

        const std::unique_ptr<TorchNetwork>& decoder = get_decoder();
        decoder->write(ie_file_path);

    }

    static std::string generate_decoder_file_path(
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
        }

        if(prefix.has_value())
            ie_file_name = prefix.value() + ie_file_name;

        return _ie_folder_path + ie_file_name;
    }

private:

    const std::string _ie_file_name;
    inline static const std::string _ie_folder_path = std::string(NEURO_EVO_CMAKE_SRC_DIR) + 
                                                      std::string("/config/decoders/");

};

} // namespace NeuroEvo

#endif
