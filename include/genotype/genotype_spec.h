#ifndef _GENOTYPE_SPEC_H_
#define _GENOTYPE_SPEC_H_

#include <util/random/distribution.h>
#include <genotype/genotype.h>
#include <genetic_operators/mutation/mutator.h>

namespace NeuroEvo {

template <typename G>
class GenotypeSpec {

public:

    GenotypeSpec(const unsigned num_genes, Distribution<G>& init_distr,
                 std::shared_ptr<Mutator<G>> mutator = nullptr) : 
        _num_genes(num_genes),
        _init_distr(init_distr),
        _mutator(mutator) 
    {
        if(!_mutator)
            std::cout << "NOTE: No mutator provided to genotype!" << std::endl;
    }

    //Generate genotype from specification
    Genotype<G>* generate_genotype() {

        std::vector<G> genes(_num_genes);

        for(std::size_t i = 0; i < genes.size(); i++)
            genes[i] = _init_distr.next();

        return new Genotype<G>(genes, _mutator);

    }

    //Generate genotype from file
    Genotype<G>* generate_genotype(const std::string& file_name) {
        //TODO: Write!
        std::cerr << "Write generate_genotype in GenotypeSpec!" << std::endl;
        exit(0);
        return nullptr;
    }

private:

    const unsigned _num_genes;

    //Distribution from which the initial genotype is sampled
    Distribution<G>& _init_distr;

    //A mutator that defines how a genotype is mutated
    std::shared_ptr<Mutator<G>> _mutator;

};

} // namespace NeuroEvo

#endif
