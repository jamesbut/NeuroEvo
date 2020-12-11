#include <phenotype/neural_network/torch_network.h>
#include <torch/nn/options/linear.h>
#include <filesystem>

namespace NeuroEvo {

TorchNetwork::TorchNetwork(const std::vector<LayerSpec>& layer_specs, const bool trace) :
    Phenotype<double>(trace),
    _net(build_network(layer_specs, std::nullopt))
{
    register_module("net", _net);
}

TorchNetwork::TorchNetwork(const std::vector<LayerSpec>& layer_specs, 
                           const std::vector<double>& init_weights, 
                           const bool trace) :
    Phenotype<double>(trace),
    _net(build_network(layer_specs, init_weights))
{
    register_module("net", _net);
}

TorchNetwork::TorchNetwork(const std::string& file_path, 
                           const std::vector<LayerSpec>& layer_specs,
                           const bool trace) :
    Phenotype<double>(trace),
    _net(read(file_path, layer_specs))
{
    register_module("net", _net);
}

std::vector<double> TorchNetwork::activate(const std::vector<double>& inputs)
{

    //Convert input to torch tensor
    torch::Tensor input_tensor = torch::zeros({1, (int64_t)inputs.size()},
                                              {torch::kFloat64});
    for(unsigned i = 0; i < inputs.size(); i++)
        input_tensor.index_put_({0, (int64_t)i}, inputs[i]);
    

    if(_trace) 
    {
        std::cout << "Inputs:" << std::endl << input_tensor << std::endl;
        std::cout << "Parameters:" << std::endl << _net->parameters() << std::endl;
    }

    //Forward pass
    const torch::Tensor output_tensor = forward(input_tensor);

    if(_trace) 
        std::cout << "Outputs:" << std::endl << output_tensor << std::endl;

    //Convert output tensor to vector
    std::vector<double> outputs;
    outputs.reserve(output_tensor.size(1));
    for(unsigned i = 0; i < output_tensor.size(1); i++)
        outputs.push_back(*output_tensor[0][i].data_ptr<double>());

    return outputs;

}

torch::Tensor TorchNetwork::forward(torch::Tensor x)
{
    return _net->forward(x);
}

std::vector<torch::Tensor> TorchNetwork::parameters(bool recurse) const
{
    return _net->parameters(recurse);
}

void TorchNetwork::zero_grad()
{
    _net->zero_grad();
}
    
//Build network from layer specifications
torch::nn::Sequential TorchNetwork::build_network(
        const std::vector<LayerSpec>& layer_specs,
        const std::optional<const std::vector<double>>& init_weights)
{

    torch::nn::Sequential net;

    //Set number of inputs and number of outputs
    _num_inputs = layer_specs[0].get_inputs_per_neuron();
    _num_outputs = layer_specs.back().get_num_neurons();

    //Build network from layer specs
    for(std::size_t i = 0; i < layer_specs.size(); i++)
    {

        //Add linear layer
        auto linear_layer = torch::nn::Linear(
            torch::nn::LinearOptions(layer_specs[i].get_inputs_per_neuron(),
                                     layer_specs[i].get_num_neurons())
        );
        net->push_back(linear_layer);

        //Add batch norm if set
        //Don't add batch norm to last layer
        if(layer_specs[i].get_batch_norm() && (i < layer_specs.size()-1))
        {
            auto batch_norm = torch::nn::BatchNorm1d(
                torch::nn::BatchNorm1dOptions(layer_specs[i].get_num_neurons())
            );
            net->push_back(batch_norm); 
        }

        //Add activation function if one is given
        if(layer_specs[i].get_activation_func_spec())
        {
            auto activation_function = layer_specs[i].get_activation_func_spec()
                                       ->create_torch_module(); 
            net->push_back(activation_function);
        }

    }

    //Set param size
    _num_params.emplace(calculate_num_net_params(net));

    //If initial weights are given, set parameters of net
    if(init_weights)
    {

        //Check init weights are same size as parameters
        if(_num_params != init_weights->size())
        {
            std::cerr << "Initial weights given to TorchNet does not match the parameter size"
                << std::endl;
            exit(0);
        }

        //Set weights in net
        std::size_t init_weight_index = 0;
        for(const auto& params : net->parameters())
        {
            const auto& flattened_params = params.flatten();
            for(int64_t i = 0; i < flattened_params.flatten().size(0); i++)
            {
                flattened_params[i] = init_weights.value()[init_weight_index];
                init_weight_index++;
            }
        }
    }

    net->to(torch::kFloat64);

    return net;

}

void TorchNetwork::reset() {}

void TorchNetwork::print(std::ostream& os) const
{
    os << _net->parameters() << std::endl;
}

void TorchNetwork::write(const std::string& file_path) const 
{
    torch::save(_net, file_path); 
}

torch::nn::Sequential TorchNetwork::read(const std::string& file_path,
                                         const std::vector<LayerSpec>& layer_specs)
{
    torch::nn::Sequential net = build_network(layer_specs, std::nullopt);

    if(std::filesystem::exists(file_path))
        torch::load(net, file_path);
    else
    {
        std::cerr << "File: " << file_path << " does not exist!" << std::endl;
        exit(0);
    }

    return net;
}

unsigned TorchNetwork::get_num_inputs() const
{
    return _num_inputs;
}

unsigned TorchNetwork::get_num_outputs() const
{
    return _num_outputs;
}

unsigned TorchNetwork::calculate_num_net_params(const torch::nn::Sequential& net) const
{

    //Iterate through params and count
    int64_t net_params_size = 0;
    for(const auto& params : net->parameters())
    {
        auto sizes = params.sizes();
        int64_t params_size = 1;
        for(const auto& size : sizes)
            params_size *= size;
        net_params_size += params_size;
    }

    return net_params_size;
}

} // namespace NeuroEvo
