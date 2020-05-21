#include <phenotype/real_vector_phenotype.h>

namespace NeuroEvo {

RealVectorPhenotype::RealVectorPhenotype(const std::vector<double>& traits) :
    _phenotype_vec(traits) {}

std::vector<double> RealVectorPhenotype::activate(const std::vector<double>& inputs) 
{
    return _phenotype_vec;
}

void RealVectorPhenotype::reset() {}

void RealVectorPhenotype::print_params() 
{
    for(const auto& trait : _phenotype_vec)
        std::cout << trait << std::endl;
}

} // namespace NeuroEvo
