#ifndef _NETWORK_MAP_H_
#define _NETWORK_MAP_H_

#include <gp_map/gp_map.h>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <phenotype/fixed_network/network.h>

namespace NeuroEvo {
namespace GPMaps {

class NetworkMap : public GPMap {

public:

    NetworkMap(Phenotypes::FixedNetworkSpec& net_spec, const std::string& file_name);

    Phenotypes::Phenotype* map(Genotypes::Genotype& genotype,
                               Phenotypes::PhenotypeSpec& pheno_spec) override;

    void print_gp_map(std::ofstream& file) override;

    std::optional<Utils::Matrix<double>> get_map() override;

protected:

    NetworkMap* clone_impl() const override { return new NetworkMap(*this); };

    Phenotypes::FixedNetwork read_network(const std::string& file_name,
                                          Phenotypes::FixedNetworkSpec& net_spec);

private:

    Phenotypes::FixedNetwork _decoder;

};

} // namespace GPMaps
} // namespace NeuroEvo

#endif
