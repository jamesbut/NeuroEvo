#include <phenotype/hebbs_network/hebbs_network.h>
#include <random>
#include <iostream>
#include <fstream>

namespace NeuroEvo {
namespace Phenotypes {

HebbsNetwork::HebbsNetwork(const HebbsNetworkSpec NET_SPEC, const bool TRACE) :
    _NET_SPEC(NET_SPEC),
    _TRACE(TRACE),
    _PRINT_WEIGHTS(NET_SPEC.PRINT_WEIGHTS),
    _WEIGHTS_FILE_NAME("../../data/indv_run_hebbs_weights"),
    _OUTPUTS_FILE_NAME("../../data/outputs") {

    remove(_WEIGHTS_FILE_NAME.c_str());
    remove(_OUTPUTS_FILE_NAME.c_str());

    create_net();

}

HebbsNetwork::HebbsNetwork(Genotypes::RealVectorGenotype& genotype, HebbsNetworkSpec& NET_SPEC,
                           const bool TRACE) :
    _NET_SPEC(NET_SPEC),
    _TRACE(TRACE),
    _PRINT_WEIGHTS(NET_SPEC.PRINT_WEIGHTS),
    _WEIGHTS_FILE_NAME("../../data/indv_run_hebbs_weights"),
    _OUTPUTS_FILE_NAME("../../data/outputs") {

    remove(_WEIGHTS_FILE_NAME.c_str());
    remove(_OUTPUTS_FILE_NAME.c_str());

    create_net();

    //Work out number of total connections
    int num_connections = 0;

    for(std::size_t i = 0; i < layers.size(); i++)
        num_connections += layers.at(i).get_number_of_connections();

    //If evolving weights as well as learning rates
    if(_NET_SPEC.EVOLVE_INIT_WEIGHTS) {

        //Split genes into 2 - learning rates and initial weights
        const auto genes = genotype.get_genes();

        const auto start = genes.begin();
        const auto end_learning_rates = genes.begin() + num_connections;
        std::vector<double> learning_rates(start, end_learning_rates);

        propogate_learning_rates(learning_rates);

        const auto start_weights = genes.begin() + num_connections;
        const auto end_weights = genes.end();
        std::vector<double> weights(start_weights, end_weights);

        propogate_weights(weights);

    } else {    //If just evolving learning rates

        std::vector<double> weights;

        //If the weights are randomly initialised or all set to some value
        if(_NET_SPEC.RANDOM_WEIGHT_INIT) {

            std::random_device rd;
            std::mt19937 eng(rd());
            std::uniform_real_distribution<> distr(0,1);

            for(unsigned int i = 0; i < num_connections; i++)
                weights.push_back(distr(eng));

        } else {

            for(unsigned int i = 0; i < num_connections; i++)
                weights.push_back(1.0);

        }

        propogate_learning_rates(genotype.get_genes());
        propogate_weights(weights);

    }

}

void HebbsNetwork::create_net() {

    for(unsigned i = 0; i < _NET_SPEC.LAYER_SPECS.size(); i++)
        layers.push_back(HebbsLayer(_NET_SPEC.LAYER_SPECS.at(i), _TRACE));

}

void HebbsNetwork::propogate_learning_rates(const std::vector<double>& learning_rates) {

    auto start = learning_rates.begin();
    auto end = learning_rates.begin();

    for(std::size_t i = 0; i < layers.size(); i++) {

        end += layers.at(i).get_number_of_connections();

        std::vector<double> tempW(start, end);
        layers.at(i).set_learning_rates(tempW);

        start += layers.at(i).get_number_of_connections();

    }

}

void HebbsNetwork::propogate_weights(const std::vector<double>& weights) {

    auto start = weights.begin();
    auto end = weights.begin();

    for(std::size_t i = 0; i < layers.size(); i++) {

        end += layers.at(i).get_number_of_connections();

        std::vector<double> tempW(start, end);
        layers.at(i).set_weights(tempW);

        start += layers.at(i).get_number_of_connections();

    }

}

std::vector<double> HebbsNetwork::activate(std::vector<double>& inputs) {

    if(_PRINT_WEIGHTS) {
        print_weights_to_file();
        print_outputs_to_file();
    }

    std::vector<double> ins = inputs;

    for(unsigned int i = 0; i < _NET_SPEC.LAYER_SPECS.size(); i++)
        ins = layers.at(i).evaluate(ins);

    return ins;

}

void HebbsNetwork::print_weights_to_file() {

    std::ofstream weight_file;
    weight_file.open(_WEIGHTS_FILE_NAME, std::fstream::app);

    for(std::size_t i = 0; i < layers.size(); i++)
        layers.at(i).print_weights_to_file(weight_file);

    weight_file << std::endl;

    weight_file.close();

}

void HebbsNetwork::print_outputs_to_file() {

    std::ofstream output_file;
    output_file.open(_OUTPUTS_FILE_NAME, std::fstream::app);

    for(std::size_t i = 0; i < layers.size(); i++)
        layers.at(i).print_outputs_to_file(output_file);

    output_file << std::endl;

    output_file.close();

}

void HebbsNetwork::print_weights() {

    for(std::size_t i = 0; i < layers.size(); i++)
        layers.at(i).print_weights();

}

void HebbsNetwork::reset() {

    for(auto& layer: layers)
        layer.reset();

}

} // namespace Phenotypes
} // namespace NeuroEvo
