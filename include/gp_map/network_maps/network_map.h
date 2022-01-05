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

    NetworkMap(NetworkBuilder& decoder_spec, std::shared_ptr<PhenotypeSpec> pheno_spec) :
        GPMap<G, T>(pheno_spec),
        _decoder(decoder_spec.build_network()) {}

    NetworkMap(const JSON& json) :
        GPMap<G, T>(Factory<PhenotypeSpec>::create(json.at({"PhenotypeSpec"}))),
        _decoder(std::make_unique<TorchNetwork>(
                    std::string(NEURO_EVO_CMAKE_SRC_DIR) +
                    json.get<std::string>({"NetworkBuilder", "file_path"}))
                ) {}

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

    JSON to_json_impl() const override
    {
        JSON json;
        json.emplace("name", "NetworkMap");
        json.emplace("decoder", _decoder->to_json().at());
        return json;
    }

    NetworkMap* clone_impl() const override = 0;

    std::unique_ptr<Phenotype<double>> _decoder;

};

} // namespace NeuroEvo

#endif
