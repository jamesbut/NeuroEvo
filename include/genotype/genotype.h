#ifndef _GENOTYPE_H_
#define _GENOTYPE_H_

/*
    A genotype is a vector of genes.
    This is a vector of doubles for now but
    will be made generic in the future
*/


#include <vector>
#include <memory>
#include <iostream>

namespace NeuroEvo {
namespace Genotypes {

class Genotype {

public:

    auto clone() const { return std::unique_ptr<Genotype>(clone_impl()); };

    inline std::vector<double>& get_genes() { return _genes; };
    inline void set_gene(const unsigned int gene_num, const double value) { _genes.at(gene_num) = value; };

    virtual inline void print_genotype(std::ofstream& file) {};
    inline void print_genotype() {
        for(const auto& gene : _genes)
            std::cout << gene << std::endl;
    }

protected:

    virtual Genotype* clone_impl() const = 0;

    std::vector<double> _genes;

};

} // namespace Genotypes
} // namespace NeuroEvo

#endif
