#ifndef _TORCH_NETWORK_H_
#define _TORCH_NETWORK_H_

/*
 * Creates an interface to a PyTorch network
 */

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/layer_spec.h>
#include <torch/torch.h>

namespace NeuroEvo {

class TorchNetwork : public Phenotype<double>, public torch::nn::Module
{

public:

    TorchNetwork(const std::vector<LayerSpec>& layer_specs, const bool trace = false);

    TorchNetwork(const std::vector<LayerSpec>& layer_specs, 
                 const std::vector<double>& init_weights, 
                 const bool trace = false);

    //Read torch network from file
    //One seems to need the layer specifications first before the network is read :/
    TorchNetwork(const std::string& file_path, const std::vector<LayerSpec>& layer_specs, 
                 const bool trace = false);

    std::vector<double> activate(const std::vector<double>& inputs) override;
    torch::Tensor forward(torch::Tensor x);

    void zero_grad() override;
    std::vector<torch::Tensor> parameters(bool recurse = true) const;

    void reset() override;
    void print_params() override;

    //Write network parameters to file
    void write(const std::string& file_path) const;
    torch::nn::Sequential read(const std::string& file_path, 
                               const std::vector<LayerSpec>& layer_specs);

    unsigned get_num_inputs() const;
    unsigned get_num_outputs() const;

private:

    torch::nn::Sequential build_network(
            const std::vector<LayerSpec>& layer_specs,
            const std::optional<const std::vector<double>>& init_weights);

    unsigned calculate_num_net_params(const torch::nn::Sequential& net) const;

    TorchNetwork* clone_impl() const override
    {
        return new TorchNetwork(*this);
    }

    torch::nn::Sequential _net;

    unsigned _num_inputs;
    unsigned _num_outputs;

    const bool _trace;


};

} // namespace NeuroEvo

#endif
