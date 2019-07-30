#include <genotype/real_vector_genotype.h>
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace NeuroEvo {
namespace Genotypes {

RealVectorGenotype::RealVectorGenotype(const unsigned int NUM_GENES) {

    std::random_device rd;
    std::mt19937 eng(rd());

    // Creates random genes from the interval [0,1] but this will
    // be definable in the future.
    std::uniform_real_distribution<> distr(0,1);

    for(size_t i = 0; i < NUM_GENES; i++)
        _genes.push_back(distr(eng));

}

RealVectorGenotype::RealVectorGenotype(const RealVectorGenotype& genotype) {

    _genes = genotype._genes;

}

RealVectorGenotype::RealVectorGenotype(const std::string& file_name) {

    std::ifstream file(file_name);
    std::string line;

    //Read whole line from file
    std::getline(file, line);

    std::stringstream ss(line);

    //Remove fitness
    std::string fitness;
    getline(ss, fitness, ',');

    while(ss.good()) {

        std::string substr;
        getline(ss, substr, ',');
        _genes.push_back(std::stod(substr));

    }

}

void RealVectorGenotype::print_genotype(std::ofstream& file) {

    //Set max precision
    file << std::setprecision(std::numeric_limits<double>::max_digits10);

    for(std::size_t i = 0; i < _genes.size(); i++)
        file << "," << _genes.at(i);

}

} // namespace Genotypes
} // namespace NeuroEvo
