#ifndef _HEBBS_NETWORK_H_
#define _HEBBS_NETWORK_H_

/*
    The weights of a Hebbs network change online
    via Hebbs rule.
    The evolutionary process optimises the learning coefficients,
    not the weights themselves.
    This implementation also supports evolving the initial weights
    as oppose to just the Hebbs coefficients.
*/

#include <phenotype/phenotype.h>
#include <phenotype/phenotype_specs/hebbs_network_spec.h>
#include <phenotype/hebbs_network/hebbs_layer.h>
#include <genotype/real_vector_genotype.h>
#include <iostream>
#include <fstream>

class HebbsNetwork : public Phenotype {

public:

    HebbsNetwork(const HebbsNetworkSpec NET_SPEC, const bool TRACE = false);

    HebbsNetwork(RealVectorGenotype& genotype, HebbsNetworkSpec& NET_SPEC, const bool TRACE = false);

    void propogate_learning_rates(const std::vector<double>& learning_rates);
    void propogate_weights(const std::vector<double>& weights);

    std::vector<double> activate(std::vector<double>& inputs);

    void print_weights();

    void reset() override;

    void print_params() override {
        for(auto& layer : layers)
            layer.print_params();
    }

protected:

    virtual HebbsNetwork* clone_impl() const override { return new HebbsNetwork(*this); };

private:

    void create_net();

    void print_weights_to_file();
    void print_outputs_to_file();

    const HebbsNetworkSpec _NET_SPEC;

    const bool _TRACE;

    std::vector<HebbsLayer> layers;

    const std::string _WEIGHTS_FILE_NAME;
    const std::string _OUTPUTS_FILE_NAME;
    const bool _PRINT_WEIGHTS;

};

#endif
