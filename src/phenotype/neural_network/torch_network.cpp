#include <phenotype/neural_network/torch_network.h>
#include <torch/nn/options/linear.h>

namespace NeuroEvo {

TorchNetwork::TorchNetwork(const std::vector<LayerSpec>& layer_specs, const bool trace) :
    _net(build_network(layer_specs)),
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

//Build network from layer specifications
torch::nn::Sequential TorchNetwork::build_network(const std::vector<LayerSpec>& layer_specs) const
{

    torch::nn::Sequential net;

    for(const auto& layer_spec : layer_specs)
    {
        //Add linear layer
        auto linear_layer = torch::nn::Linear(
                torch::nn::LinearOptions(layer_spec.get_inputs_per_neuron(),
                                         layer_spec.get_num_neurons())
                );
        net->push_back(torch::nn::AnyModule(linear_layer));

        //Add activation function
        auto activation_function = layer_spec.get_activation_func_spec()->create_torch_module(); 
        net->push_back(activation_function);

    }

    return net;

}

void TorchNetwork::reset() {}

void TorchNetwork::print_params() {}

} // namespace NeuroEvo
