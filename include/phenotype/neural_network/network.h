#ifndef _NETWORK_H_
#define _NETWORK_H_

/*
    Defines a network as a vector of layers
*/

#include <phenotype/phenotype.h>
#include <phenotype/neural_network/layer.h>

namespace NeuroEvo {

class Network : public Phenotype<double>
{

public:

    Network(const bool trace = false);
    Network(const Network& network);

    virtual void create_net(const std::vector<LayerSpec>& layer_specs);

    void propogate_weights(const std::vector<double>& weights);
    virtual void propogate_learning_rates(const std::vector<double>& learning_rates) {}

    virtual std::vector<double> activate(const std::vector<double>& inputs) override;

    void reset() override;

    void set_trace(const bool trace) override;

    std::vector<double> get_weights() const;
    const std::vector<std::unique_ptr<Layer>>& get_layers() const;
    const std::shared_ptr<ActivationFunction> get_final_layer_activ_func() const;

protected:

    virtual Network* clone_impl() const override
    {
        return new Network(*this);
    };

    std::vector<double> propogate(const std::vector<double>& inputs);

    std::vector<std::unique_ptr<Layer>> _layers;

private:

    void print(std::ostream& os) const override;

};

} // namespace NeuroEvo

#endif
