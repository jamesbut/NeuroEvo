#include <gp_map/matrix_map.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <phenotype/neural_network/network.h>
#include <phenotype/vector_phenotype.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <iterator>

namespace NeuroEvo {

MatrixMap::MatrixMap(const unsigned genotype_size,
                     const unsigned phenotype_size,
                     const PhenotypeSpec* pheno_spec) :
    GPMap<double, double>(pheno_spec),
    _interaction_matrix(phenotype_size, genotype_size) 
{

    //Create the identity interaction matrix
    for(unsigned i = 0; i < _interaction_matrix.get_height(); i++)
        for(unsigned j = 0; j < _interaction_matrix.get_width(); j++) 
            if(i == j)
                _interaction_matrix.set(i, j, 1.0);

}

MatrixMap::MatrixMap(const std::string& file_name, const PhenotypeSpec* pheno_spec) :
    GPMap<double, double>(pheno_spec),
    _interaction_matrix(read_matrix(file_name)) {}

Phenotype<double>* MatrixMap::map(Genotype<double>& genotype)
{

    const Matrix<double> genes(genotype.genes());

    Matrix<double> traits = _interaction_matrix * genes;
    std::vector<double> traits_vector = traits.get_vector();

    //TODO: This is hacky, I want to change this
   auto net_builder = dynamic_cast<const NetworkBuilder*>(_pheno_spec.get());

    if(net_builder != nullptr)
    {

        Network* network = new Network(net_builder->get_trace());
        network->create_net(net_builder->get_layer_specs());
        network->propogate_weights(traits.get_vector());
        return network;

    } else
        return new VectorPhenotype<double>(traits_vector);
}

void MatrixMap::print_gp_map(std::ofstream& file) const {

    file << "\n\n";

    file << std::setprecision(std::numeric_limits<double>::max_digits10);

    for(unsigned i = 0; i < _interaction_matrix.get_height(); i++) {
        for(unsigned j = 0; j < _interaction_matrix.get_width(); j++) {
            if(i == 0 && j== 0)
                file << _interaction_matrix.at(i, j);
            else
                file << " " << _interaction_matrix.at(i, j);
        }
        file << "\n";
    }

}

Matrix<double> MatrixMap::read_matrix(const std::string& file_name) {

    std::vector<std::vector<double>> vector_matrix;

    std::ifstream file(file_name);
    std::string line;

    if(file.is_open()) {

        //If the file is an org file as oppose to a .im file,
        //remove the lines above the matrix
        if(file_name.find(".im") == std::string::npos) {

            //Not interested in first 2 lines, that is genotype information
            std::getline(file, line);
            std::getline(file, line);

        }

        while(getline(file, line)) {

            //Split line by white spaces
            std::istringstream iss(line);
            std::vector<std::string> split_string{std::istream_iterator<std::string>{iss},
                                                  std::istream_iterator<std::string>{}};

            //Convert strings to doubles
            std::vector<double> matrix_values_row(split_string.size());

            for(std::size_t i = 0; i < split_string.size(); i++)
                matrix_values_row.at(i) = std::stod(split_string.at(i));

            vector_matrix.push_back(matrix_values_row);

        }

        file.close();

    } else {

        std::cerr << "Could not open: " << file_name << std::endl;
        exit(EXIT_FAILURE);

    }

    return Matrix<double>(vector_matrix);

}

} // namespace NeuroEvo
