#include <util/models/trainable_model.h>

namespace NeuroEvo {

const std::unique_ptr<TorchNetwork>& TrainableModel::get_model() const
{
    return _model;
}

double TrainableModel::get_loss() const
{
    return _loss;
}

torch::Tensor TrainableModel::forward(const torch::Tensor& input, const bool trace) const
{
    const torch::Tensor output = _model->forward(input);

    if(trace)
    {
        std::cout << "Model input: " << std::endl;
        std::cout << input << std::endl;
        std::cout << "Model output: " << std::endl;
        std::cout << output << std::endl;
    }

    return output;
}

void TrainableModel::write_model(const std::optional<const unsigned>& prefix) const
{
    //Check decoders directory exists first
    if(!boost::filesystem::exists(_model_folder_path))
        boost::filesystem::create_directory(_model_folder_path);

    //Add epoch number at beginning of file name if one is given
    std::string model_file_name = _model_file_name;  
    if(prefix.has_value())
        model_file_name = std::to_string(prefix.value()) + "_" + model_file_name;
    const std::string model_file_path = _model_folder_path + model_file_name;

    _model->write(model_file_path);

}

std::string TrainableModel::generate_ie_file_path(
    const unsigned ie_type,
    const std::optional<const std::string>& prefix)
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

void TrainableModel::print_params() const
{
    std::cout << _model->parameters() << std::endl;
}

} // namespace NeuroEvo
