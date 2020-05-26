#ifndef _GENOTYPE_H_
#define _GENOTYPE_H_

/*
 * A genotype is a vector of templated values that can be mutated according to some mutator
 */

#include <vector>
#include <iostream>
#include <genetic_operators/mutation/mutator.h>

namespace NeuroEvo {

template <typename G>
class Genotype {

public:

    Genotype(const std::vector<G>& genes, std::shared_ptr<Mutator<G>> mutator = nullptr) :
        _genes(genes),
        _mutator(mutator) {}

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

    friend std::ostream& operator<<(std::ostream& os, const Genotype& genotype) 
    {
        for(std::size_t i = 0; i < genotype._genes.size(); i++)
        {
            os << genotype._genes[i];
            if(i != genotype._genes.size()-1)
                os << " ";
        }
        return os;
    }

protected:

    std::vector<G> _genes;
    std::shared_ptr<Mutator<G>> _mutator;

};

} // namespace NeuroEvo

#endif
