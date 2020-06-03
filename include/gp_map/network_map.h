#ifndef _NETWORK_MAP_H_
#define _NETWORK_MAP_H_

#include <gp_map/gp_map.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <phenotype/neural_network/network.h>

/*
 * A network map passes the genotype through a network
 * and the output of that network are the weights
 * of the phenotype network.
 * This is only defined for double genes but can be made
 * generic in future.
 */

namespace NeuroEvo {

class NetworkMap : public GPMap<double, double>
{

public:

    NetworkMap(NetworkBuilder& net_spec, const std::string& file_name);

    Phenotype<double>* map(Genotype<double>& genotype,
                           PhenotypeSpec<double, double>& pheno_spec) override;

    void print_gp_map(std::ofstream& file) override;

    std::optional<Utils::Matrix<double>> get_map() override;

protected:

    NetworkMap* clone_impl() const override { return new NetworkMap(*this); };

    Network read_network(const std::string& file_name,
                         NetworkBuilder& net_spec);

private:

    Network _decoder;

};

} // namespace NeuroEvo

#endif
