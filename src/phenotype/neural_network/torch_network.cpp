#include <phenotype/neural_network/torch_network.h>
#include <torch/nn/options/linear.h>

namespace NeuroEvo {

TorchNetwork::TorchNetwork(const std::vector<LayerSpec>& layer_specs, const bool trace) :
    _net(build_network(layer_specs, std::nullopt)),
    _trace(trace) {}

TorchNetwork::TorchNetwork(const std::vector<LayerSpec>& layer_specs, 
                           const std::vector<double>& init_weights, 
                           const bool trace) :
    _net(build_network(layer_specs, init_weights)),
    _trace(trace) {}

std::vector<double> TorchNetwork::activate(const std::vector<double>& inputs)
{

    //Convert input to torch tensor
    torch::Tensor input_tensor = torch::zeros(inputs.size());
    for(unsigned i = 0; i < inputs.size(); i++)
        input_tensor[i] = inputs[i];

    if(_trace) 
    {
        std::cout << "Inputs:" << std::endl << input_tensor << std::endl;
        std::cout << "Parameters:" << std::endl << _net->parameters() << std::endl;
    }

    //Forward pass
    const torch::Tensor output_tensor = forward(input_tensor);

    if(_trace) std::cout << "Outputs:" << std::endl << output_tensor << std::endl;

    //Convert output tensor to vector
    std::vector<double> outputs;
    outputs.reserve(output_tensor.size(0));
    for(unsigned i = 0; i < output_tensor.size(0); i++)
        outputs.push_back(*output_tensor[i].data_ptr<float>());

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

    //Set number of inputs
    _num_inputs = layer_specs[0].get_inputs_per_neuron();

    //Build network from layer specs
    for(const auto& layer_spec : layer_specs)
    {
        //Add linear layer
        auto linear_layer = torch::nn::Linear(
                torch::nn::LinearOptions(layer_spec.get_inputs_per_neuron(),
                                         layer_spec.get_num_neurons())
                );
        net->push_back(torch::nn::AnyModule(linear_layer));

        //Add activation function if one is given
        if(layer_spec.get_activation_func_spec())
        {
            auto activation_function = layer_spec.get_activation_func_spec()
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
            std::cerr << "Initial weights given to TorchNet does not match the parameter size" <<
                std::endl;
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

    return net;

}

void TorchNetwork::reset() {}

void TorchNetwork::print_params() {}

unsigned TorchNetwork::get_num_inputs() const
{
    return _num_inputs;
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
