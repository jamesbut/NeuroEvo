#ifndef _MUTATOR_H_
#define _MUTATOR_H_

/*
 * A Mutator describes how a gene is mutated.
 * One can inherit from this class in order to define how to mutate on a particular
 * type in a particular way.
 */

#include <util/random/uniform_real_distribution.h>

namespace NeuroEvo {

template <typename G>
class Mutator {

    public:

        Mutator(const double mutation_rate) :
            _mutation_rate(mutation_rate),
            _mutation_distr(0., 1.) {}

        virtual ~Mutator() = default;

        void mutate(std::vector<G>& genes) {

            for(std::size_t i = 0; i < genes.size(); i++)
                if(should_mutate())
                    genes[i] = mutate_gene(genes[i]);

        }

    protected:
    
        virtual G mutate_gene(G gene) = 0;

        //The probability that a single gene is mutated
        const double _mutation_rate;

        //Distribution used to sample from to determine
        //whether a gene should mutate or not
        UniformRealDistribution _mutation_distr;

    private:

        //Determines whether gene should mutate
        bool should_mutate() {
            return _mutation_distr.next() < _mutation_rate;
        }

};

} // namespace NeuroEvo

#endif