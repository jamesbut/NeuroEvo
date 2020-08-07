#ifndef _GENOTYPE_H_
#define _GENOTYPE_H_

/*
 * A genotype is a vector of templated values that can be mutated according to some mutator
 */

#include <vector>
#include <iostream>
#include <genetic_operators/mutation/mutator.h>
#include <memory>

namespace NeuroEvo {

template <typename G>
class Genotype {

public:

    Genotype() = default;

    Genotype(const std::vector<G>& genes, std::shared_ptr<Mutator<G>> mutator = nullptr) :
        _genes(genes),
        _mutator(mutator) {}

    Genotype(const std::vector<G>& genes, const double fitness, 
             std::shared_ptr<Mutator<G>> mutator = nullptr) :
        _genes(genes),
        _mutator(mutator),
        _fitness(fitness) {}

    auto clone() const
    {
        return std::unique_ptr<Genotype<G>>(new Genotype<G>(*this));
    }

    const std::vector<G>& genes() const 
    {
        return _genes;
    }

    void mutate() 
    {
        if(_mutator)
            _mutator->mutate(_genes);
    }

    const std::optional<double>& get_fitness() const
    {
        return _fitness;
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

    std::vector<G> _genes;
    std::shared_ptr<Mutator<G>> _mutator;

    //This is only here for ease - it is mainly used when reading a genotype from file
    //and one wants quick access to the fitness that this genotype had when it was tested
    //in a previous phenotype.
    //This is not necessarily reliable though, do not use unless you know what you are
    //doing.
    std::optional<double> _fitness;

};

} // namespace NeuroEvo

#endif
