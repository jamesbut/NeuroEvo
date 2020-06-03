#include <gp_map/network_map.h>
#include <sstream>
#include <fstream>
#include <iterator>

namespace NeuroEvo {

NetworkMap::NetworkMap(NetworkBuilder& net_spec, const std::string& file_name) :
    _decoder(read_network(file_name, net_spec)) {}

Phenotype<double>* NetworkMap::map(Genotype<double>& genotype,
                                   PhenotypeSpec<double, double>& pheno_spec) 
{

    //Query the decoder with the genes - it will output
    //the weights of the phenotype
    std::vector<double> traits = _decoder.activate(genotype.genes());

    //TODO: This is hacky, I want to change this
    NetworkBuilder* net_builder = dynamic_cast<NetworkBuilder*>(&pheno_spec);

    if(net_builder != nullptr)
    {
        Network* network = new Network(net_builder->get_trace());
        network->create_net(net_builder->get_layer_specs());
        network->propogate_weights(traits);
        return network;

    } else 
    {
        std::cerr << "Could not create Fixed Network from decoded traits" << std::endl;
        exit(EXIT_FAILURE);
    }

}

void NetworkMap::print_gp_map(std::ofstream& file) 
{

    //TODO: Write this
    std::cerr << "Write print_gp_map in NetworkMap!" << std::endl;
    exit(0);

}

std::optional<Utils::Matrix<double>> NetworkMap::get_map() 
{

    //TODO: Write this
    std::cerr << "Write get_map in NetworkMap" << std::endl;
    exit(0);

}

Network NetworkMap::read_network(const std::string& file_name,
                                 NetworkBuilder& net_builder) 
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

    Network* network = new Network(net_builder.get_trace());
    network->create_net(net_builder.get_layer_specs());
    network->propogate_weights(weights);
    return network;

}

} // namespace NeuroEvo
