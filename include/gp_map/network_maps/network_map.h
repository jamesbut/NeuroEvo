#ifndef _NETWORK_MAP_H_
#define _NETWORK_MAP_H_

#include <gp_map/gp_map.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <phenotype/neural_network/network.h>
#include <sstream>
#include <iterator>

/*
 * A network map passes the genotype through a network
 * and the outputs of that network are used to build
 * the phenotype.
 */

namespace NeuroEvo {

template<typename G, typename T>
class NetworkMap : public GPMap<G, T>
{

public:

    NetworkMap(NetworkBuilder& decoder_spec, PhenotypeSpec* pheno_spec) :
        GPMap<G, T>(pheno_spec),
        _decoder(decoder_spec.build_network()) {}

    NetworkMap(NetworkBuilder& decoder_spec, PhenotypeSpec* pheno_spec, 
               const std::string& file_name) :
        GPMap<G, T>(pheno_spec),
        _decoder(read_network(file_name, decoder_spec)) {}

    NetworkMap(const NetworkMap& network_map) :
        GPMap<G, T>(network_map),
        _decoder(network_map._decoder->clone_phenotype()) {}

    Phenotype<T>* map(Genotype<G>& genotype) override = 0;

    void print(std::ostream& os) const override 
    {
        std::cout << *_decoder << std::endl;
    }

    const std::unique_ptr<Phenotype<double>>& get_decoder() const 
    {
        return _decoder;
    }

protected:

    NetworkMap* clone_impl() const override = 0;

    Phenotype<double>* read_network(const std::string& file_name,
                                    NetworkBuilder& decoder_spec)
    {

        std::stringstream file_path;
        file_path << NEURO_EVO_CMAKE_SRC_DIR << "/config/decoders/" << file_name;
        std::ifstream file(file_path.str());

        std::string line;
        std::vector<double> weights;

        if(file.is_open()) 
        {

            while(getline(file, line)) 
            {

                //Split line by white spaces
                std::istringstream iss(line);
                std::vector<std::string> split_string{std::istream_iterator<std::string>{iss},
                                                      std::istream_iterator<std::string>{}};

                for(const auto& weight : split_string)
                    weights.push_back(std::stod(weight));

            }

        } else 
        {

            std::cout << "Could not open: " << file_path.str() << std::endl;
            exit(EXIT_FAILURE);

        }

        Network* network = new Network(decoder_spec.get_trace());
        network->create_net(decoder_spec.get_layer_specs());
        network->propogate_weights(weights);
        return network;

    }

    std::unique_ptr<Phenotype<double>> _decoder;

};

} // namespace NeuroEvo

#endif
