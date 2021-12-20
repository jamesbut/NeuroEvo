#ifndef _GENOTYPE_H_
#define _GENOTYPE_H_

/*
 * A genotype is a vector of templated values that can be mutated according to some mutator
 */

#include <vector>
#include <iostream>
#include <genetic_operators/mutation/mutator.h>
#include <memory>
#include <fstream>
#include <sstream>
#include <data/json.h>

namespace NeuroEvo {

template <typename G>
class Genotype {

public:

    Genotype() = default;

    Genotype(const std::vector<G>& genes) :
        _genes(genes) {}

    Genotype(const unsigned num_genes, std::unique_ptr<Distribution<G>>& gene_distr) :
        _genes(generate_genes_from_distr(num_genes, gene_distr)) {}

    Genotype(const std::vector<G>& genes, const double fitness) :
        _fitness(fitness),
        _genes(genes) {}

    auto clone() const
    {
        return std::unique_ptr<Genotype<G>>(new Genotype<G>(*this));
    }

    std::vector<G>& genes()
    {
        return _genes;
    }

    const std::optional<double>& get_fitness() const
    {
        return _fitness;
    }

    JSON to_json() const
    {
        JSON json;
        json.emplace("genes", _genes);
        return json;
    }

    friend std::ostream& operator<<(std::ostream& os, const Genotype& genotype)
    {
        for(std::size_t i = 0; i < genotype._genes.size(); i++)
        {
            os << genotype._genes[i];
            if(i != genotype._genes.size()-1)
                os << ",";
        }
        return os;
    }

private:

    std::vector<G> generate_genes_from_distr(
        const unsigned num_genes,
        std::unique_ptr<Distribution<G>>& gene_distr) const
    {
        std::vector<G> genes(num_genes);
        for(unsigned i = 0; i < num_genes; i++)
            genes[i] = gene_distr->next();
        return genes;
    }

    //This is only here for ease - it is mainly used when reading a genotype from file
    //and one wants quick access to the fitness that this genotype had when it was tested
    //in a previous phenotype.
    //This is not necessarily reliable though, do not use unless you know what you are
    //doing.
    std::optional<double> _fitness;

    std::vector<G> _genes;

};

} // namespace NeuroEvo

#endif
