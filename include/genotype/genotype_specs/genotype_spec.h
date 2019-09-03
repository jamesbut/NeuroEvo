#ifndef _GENOTYPE_SPEC_H_
#define _GENOTYPE_SPEC_H_

#include <util/random/distribution.h>
#include <genotype/real_vector_genotype.h>

namespace NeuroEvo {
namespace Genotypes {

struct GenotypeSpec {

    GenotypeSpec(const unsigned num_genes, Utils::Distribution& distr) :
        _num_genes(num_genes),
        _distr(distr) {}

    Genotype* generate_genotype() {

        std::vector<double> genes(_num_genes);

        // Generate genes according to the distribution
        for(unsigned i = 0; i < genes.size(); i++)
            genes.at(i) = _distr.next();

        return new RealVectorGenotype(genes);

    }

    Genotype* generate_genotype(const std::string& file_name) {

        return new RealVectorGenotype(file_name);

    }

    const unsigned _num_genes;
    Utils::Distribution& _distr;

};

} // namespace Genotypes
} // namespace NeuroEvo

#endif
