#include <phenotype/neural_network/torch_network.h>
#include <torch/nn/options/linear.h>
#include <filesystem>
#include <util/exceptions/not_implemented_exception.h>
#include <util/string.h>
#include <util/torch/tensor_utils.h>

namespace NeuroEvo {

TorchNetwork::TorchNetwork(const std::vector<LayerSpec>& layer_specs,
                           const bool trace) :
    NetworkBase(trace),
    _net(build_network(layer_specs, std::nullopt)),
    _layer_specs(layer_specs)
{
    register_module("net", _net);
}

TorchNetwork::TorchNetwork(const std::vector<LayerSpec>& layer_specs,
                           const std::vector<double>& init_weights,
                           const bool trace) :
    NetworkBase(trace),
    _net(build_network(layer_specs, init_weights)),
    _layer_specs(layer_specs)
{
    register_module("net", _net);
}

TorchNetwork::TorchNetwork(const std::string& file_path,
                           const bool trace) :
    NetworkBase(trace),
    _net(read(file_path))
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

    //Save final layer activation function
    _final_layer_activ_func.reset(
        layer_specs.back().get_activation_func_spec()->create_activation_function());

    //Set param size
    _num_params.emplace(calculate_num_net_params(net));

    //If initial weights are given, set parameters of net
    if(init_weights.has_value())
    {
        //Check init weights are same size as parameters
        if(_num_params != init_weights->size())
        {
            std::cerr << "Initial weights given to TorchNet does not match the "
                "parameter size" << std::endl;
            exit(0);
        }

        //Set initial weights of network
        std::size_t weight_index = 0;
        for(const auto& params : net->parameters())
        {
            //Determine number of parameters in parameter tensor
            unsigned num_params = 1;
            for(auto it = params.sizes().begin(); it != params.sizes().end(); it++)
                num_params *= *it;

            //Take a slice from init weights vector of num params size
            std::vector<double> init_weights_slice =
                std::vector<double>(init_weights->begin() + weight_index,
                                    init_weights->begin() + weight_index + num_params);
            weight_index += num_params;

            //Set data
            //I don't know how I set this when params is a const reference :/
            params.set_data(vector_to_tensor(init_weights_slice,
                                             params.size(0),
                                             params.size(1)));

        }

        /*
        //This technique also seems to work
        net->parameters()[0].set_requires_grad(false);
        net->parameters()[0].index_put_({0, 0}, 1.);
        net->parameters()[0].set_requires_grad(true);
        */
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

    //Also save layer specs
    JSON layers_json;
    for(std::size_t i = 0; i < _layer_specs.size(); i++)
        layers_json.emplace("LayerSpec" + std::to_string(i),
                            _layer_specs.at(i).to_json());

    layers_json.save_to_file(get_layer_specs_file_path(file_path));
}

torch::nn::Sequential TorchNetwork::read(const std::string& file_path)
{
    //Read and build layer specs
    JSON layer_specs_json(get_layer_specs_file_path(file_path));
    _layer_specs = LayerSpec::build_layer_specs(layer_specs_json);

    torch::nn::Sequential net = build_network(_layer_specs, std::nullopt);

    if(std::filesystem::exists(file_path))
        torch::load(net, file_path);
    else
        throw std::invalid_argument("File: " + file_path + " does not exist!");

    return net;
}

std::string TorchNetwork::get_layer_specs_file_path(const std::string& file_path) const
{
    return remove_extension(file_path) + "_layer_specs.json";
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

JSON TorchNetwork::to_json_impl() const
{
    throw NotImplementedException("TorchNetwork::to_json_impl()");
}

TorchNetwork* TorchNetwork::clone_impl() const
{
    return new TorchNetwork(*this);
}

std::vector<double> TorchNetwork::get_params() const
{
    std::vector<torch::Tensor> torch_params = parameters();

    //Convert parameters into a 1d vector of doubles
    std::vector<double> param_vec;
    for(const auto& param_tensor : torch_params)
    {
        const std::vector<double> params = tensor_to_vector(param_tensor);
        param_vec.insert(param_vec.end(), params.begin(), params.end());
    }

    return param_vec;
}

} // namespace NeuroEvo
