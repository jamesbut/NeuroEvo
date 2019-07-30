#include <gp_map/matrix_map.h>
#include <phenotype/fixed_network/network.h>
#include <phenotype/real_vector_phenotype.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>

namespace NeuroEvo {
namespace GPMaps {

MatrixMap::MatrixMap(const unsigned genotype_size,
                     const unsigned phenotype_size) :
    _interaction_matrix(phenotype_size, genotype_size) {

    //Create the identity interaction matrix
    for(unsigned i = 0; i < _interaction_matrix.get_height(); i++)
        for(unsigned j = 0; j < _interaction_matrix.get_width(); j++) {
            if(i == j)
                _interaction_matrix.set(i, j, 1.0);
        }

}

MatrixMap::MatrixMap(const unsigned genotype_size,
                     const unsigned phenotype_size,
                     const std::string& file_name) :
    _interaction_matrix(phenotype_size, genotype_size, read_matrix(file_name)) {}

Phenotypes::Phenotype* MatrixMap::map(Genotypes::Genotype& genotype,
                                      Phenotypes::PhenotypeSpec& pheno_spec) {

    const Utils::Matrix<double> genes(genotype.get_genes());

    Utils::Matrix<double> traits = _interaction_matrix * genes;

    std::vector<double> traits_vector = traits.get_vector();

    //TODO: This is hacky, I want to change this
   Phenotypes::FixedNetworkSpec* net_spec = dynamic_cast<Phenotypes::FixedNetworkSpec*>(&pheno_spec);

    if(net_spec != nullptr)
        return new Phenotypes::FixedNetwork(traits_vector, *net_spec);
    else
        return new Phenotypes::RealVectorPhenotype(traits_vector);
}

void MatrixMap::print_gp_map(std::ofstream& file) {

    file << "\n";

    file << std::setprecision(std::numeric_limits<double>::max_digits10);

    for(unsigned i = 0; i < _interaction_matrix.get_height(); i++)
        for(unsigned j = 0; j < _interaction_matrix.get_width(); j++) {
            if(i == 0 && j== 0)
                file << _interaction_matrix.at(i, j);
            else
                file << "," << _interaction_matrix.at(i, j);
        }

}

std::vector<double> MatrixMap::read_matrix(const std::string& file_name) {

    std::ifstream file(file_name);
    std::string line;

    //Not interested in first line, that is genotype information
    std::getline(file, line);
    std::getline(file, line);

    std::stringstream ss(line);
    std::vector<double> matrix_values;

    while(ss.good()) {

        std::string substr;
        getline(ss, substr, ',');
        matrix_values.push_back(std::stod(substr));

    }

    return matrix_values;

}

} // namespace GPMaps
} // namespace NeuroEvo
