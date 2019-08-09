#include <gp_map/matrix_map.h>
#include <phenotype/fixed_network/network.h>
#include <phenotype/real_vector_phenotype.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <iterator>

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

MatrixMap::MatrixMap(const std::string& im_file_name) :
    _interaction_matrix(read_matrix_im_file(im_file_name)) {}

MatrixMap::MatrixMap(const unsigned genotype_size,
                     const unsigned phenotype_size,
                     const std::string& org_file_name) :
    _interaction_matrix(phenotype_size, genotype_size, read_matrix_org_file(org_file_name)) {}

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

std::vector<double> MatrixMap::read_matrix_org_file(const std::string& org_file_name) {

    std::ifstream file(org_file_name);
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

Utils::Matrix<double> MatrixMap::read_matrix_im_file(const std::string& im_file_name) {

    std::vector<std::vector<double> > matrix;

    //Open and read from .im file
    std::ifstream matrix_file;
    matrix_file.open(im_file_name);

    if(matrix_file.is_open()) {

        std::string line;

        while(getline(matrix_file, line)) {

            //Split line by white spaces
            std::istringstream iss(line);
            std::vector<std::string> split_string{std::istream_iterator<std::string>{iss},
                                                  std::istream_iterator<std::string>{}};

            //Convert strings to doubles
            std::vector<double> matrix_values_row(split_string.size());

            for(std::size_t i = 0; i < split_string.size(); i++)
                matrix_values_row.at(i) = std::stod(split_string.at(i));

            matrix.push_back(matrix_values_row);

        }

        matrix_file.close();

    } else {

        std::cerr << "Could not open: " << im_file_name << std::endl;
        exit(EXIT_FAILURE);

    }

    return Utils::Matrix<double>(matrix);

}

} // namespace GPMaps
} // namespace NeuroEvo
