#include <util/models/trainable_model.h>

namespace NeuroEvo {

TrainableModel::TrainableModel(NetworkBuilder& model_net_builder,
                               const std::string& model_name) :
    _model(dynamic_cast<TorchNetwork*>(model_net_builder.build_network())),
    _model_name(model_name) {}

const std::unique_ptr<TorchNetwork>& TrainableModel::get_model() const
{
    return _model;
}

double TrainableModel::get_loss() const
{
    return _loss;
}

torch::Tensor TrainableModel::forward(const torch::Tensor& input, const bool trace)
    const
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

void TrainableModel::write_model(
    const std::string& model_dir,
    const JSON& config,
    const std::optional<const unsigned>& prefix
) const
{
    // Determine model file name
    const std::string model_file_name =
        calculate_model_file_name(model_dir, prefix);

    // Dump training configuration json
    const std::string config_file_path = model_dir +
                                         remove_extension(model_file_name) +
                                         "_config";
    config.save_to_file(config_file_path);

    // Write model
    _model->write(model_dir + model_file_name);
}

std::string TrainableModel::calculate_model_file_name(
    const std::string& model_dir,
    const std::optional<const unsigned>& prefix
) const
{
    std::string model_file_name = _model_name;

    // Search for other IE model files in directory
    std::vector<std::string> saved_model_file_names;
    for(const auto& file : std::filesystem::directory_iterator(model_dir))
        if(file.path().string().ends_with(".pt"))
        {
            // Calculate just the model file name with extension removed
            const std::string file_name =
                remove_extension(split_string(file.path().string(), "/").back());

            // Filter for model of this specific type, i.e. ae, vae, or gan
            if(split_string(file_name, "_").front().compare(model_file_name) == 0)
                saved_model_file_names.push_back(file_name);
        }

    // Determine new model number
    unsigned new_model_num = 0;
    if(!saved_model_file_names.empty())
    {
        std::vector<unsigned> current_model_nums;
        current_model_nums.reserve(saved_model_file_names.size());

        // Retrieve current model numbers
        for(const auto& file_name : saved_model_file_names)
            current_model_nums.push_back(
                std::stoi(split_string(file_name, "_").back())
            );

        // Order model numbers
        std::sort(current_model_nums.begin(), current_model_nums.end());

        // Add one to previous model number
        new_model_num = current_model_nums.back() + 1;
    }

    //Add epoch number at beginning of file name if one is given
    if(prefix.has_value())
        model_file_name = std::to_string(prefix.value()) + "_" + model_file_name;

    // Append model number and .pt extension
    return model_file_name + "_" + std::to_string(new_model_num) + ".pt";
}


void TrainableModel::print_params() const
{
    std::cout << _model->parameters() << std::endl;
}

TrainableModel::LocalMinChecker::LocalMinChecker(const unsigned test_epoch,
                                                 const double plateau_loss) :
    _test_epoch(test_epoch),
    _plateau_loss(plateau_loss) {}

bool TrainableModel::LocalMinChecker::is_in_local_min(const unsigned current_epoch,
                                                      const double current_loss)
{
    //Do not check unless the current epoch is at least the test epoch
    if(current_epoch < _test_epoch)
        return false;

    //If the current loss is less than what we would expect to see the plateaued loss
    //at a local minima, we assume it is not stuck in a local min
    if(current_loss < _plateau_loss)
        return false;

    return true;

}

} // namespace NeuroEvo
