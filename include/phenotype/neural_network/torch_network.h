#ifndef _TORCH_NETWORK_H_
#define _TORCH_NETWORK_H_

/*
 * Creates an interface to a PyTorch network
 */

#include <phenotype/neural_network/network_base.h>
#include <phenotype/phenotype_specs/layer_spec.h>
#include <torch/torch.h>

namespace NeuroEvo {

class TorchNetwork : public NetworkBase, public torch::nn::Module
{

public:

    TorchNetwork(const std::vector<LayerSpec>& layer_specs, const bool trace = false);

    TorchNetwork(const std::vector<LayerSpec>& layer_specs,
                 const std::vector<double>& init_weights,
                 const bool trace = false);

    //Read torch network from file
    TorchNetwork(const std::string& file_path,
                 const bool trace = false);

    std::vector<double> activate(const std::vector<double>& inputs) override;
    torch::Tensor forward(torch::Tensor x);

    void zero_grad() override;
    std::vector<torch::Tensor> parameters(bool recurse = true) const;

    void reset() override;

    //Write network parameters to file
    void write(const std::string& file_path) const;
    torch::nn::Sequential read(const std::string& file_path);

    void print(std::ostream& os) const override;

private:

    torch::nn::Sequential build_network(
            const std::vector<LayerSpec>& layer_specs,
            const std::optional<const std::vector<double>>& init_weights);

    unsigned calculate_num_net_params(const torch::nn::Sequential& net) const;

    std::string get_layer_specs_file_path(const std::string& file_path) const;

    JSON to_json_impl() const override;
    TorchNetwork* clone_impl() const override;

    std::vector<double> get_params() const override;

    torch::nn::Sequential _net;

    //Save layer specs in case of writing to file
    std::vector<LayerSpec> _layer_specs;

};

} // namespace NeuroEvo

#endif
