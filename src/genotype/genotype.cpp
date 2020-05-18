#include <genotype/genotype.h>

namespace NeuroEvo {
namespace Genotypes {

Genotype::Genotype(const std::vector<Gene>& genes, const GeneType gene_type) :
    _genes(genes),
    _gene_type(gene_type) {}

const std::vector<Gene>& Genotype::get_genes() const {
    return _genes;
}

const GeneType Genotype::gene_type() const {
    return _gene_type;
}

std::ostream& operator<<(std::ostream& os, const Genotype& genotype) {

    //TODO: I think I will have to print the value of the gene - I think this will print
    //the object reference
    for(const auto& gene : _genes)
        os << gene << " ";
    os << std::endl; 

    return os;

}

} // namespace Genotypes
} // namespace NeuroEvo
