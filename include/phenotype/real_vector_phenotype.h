#ifndef _REAL_VECTOR_PHENOTYPE_H_
#define _REAL_VECTOR_PHENOTYPE_H_

/*
    The real vector phenotype is not a neural network
    it is just a vector. This is useful for evolution
    on general optimisation procedures that do not use
    networks.
    The activate function in this phenotype therefore
    just returns the phenotypical traits.
*/

#include <phenotype/phenotype.h>
#include <genotype/real_vector_genotype.h>

namespace NeuroEvo {
namespace Phenotypes {

class RealVectorPhenotype : public Phenotype {

public:

    RealVectorPhenotype(const std::vector<double>& traits);

    virtual std::vector<double> activate(std::vector<double>& inputs) override;

    void reset() override;

    void print_params() override;

protected:

    virtual RealVectorPhenotype* clone_impl() const override { return new RealVectorPhenotype(*this); };

private:

    const std::vector<double> _phenotype_vec;

};

} // namespace Phenotypes
} // namespace NeuroEvo

#endif
