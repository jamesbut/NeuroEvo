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

torch::nn::Sequential TorchNetwork::build_network(const std::vector<LayerSpec>& layer_specs) const
{
    const std::int64_t num_inputs = 1;
    const std::int64_t num_outputs = 1;

    std::vector<torch::nn::Module> modules;

    torch::nn::Sequential net(
        (torch::nn::Linear(torch::nn::LinearOptions(num_inputs, num_outputs))),
        torch::nn::ReLU()
    );

    return net;

}

void TorchNetwork::reset() {}

void TorchNetwork::print_params() {}

} // namespace NeuroEvo
