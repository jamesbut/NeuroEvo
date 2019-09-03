#ifndef _GENOME_H_
#define _GENOME_H_

/*
    This is the standard genotype for now and is a
    vector of real numbers.
*/

#include <genotype/genotype.h>

namespace NeuroEvo {
namespace Genotypes {

class RealVectorGenotype : public Genotype {

public:

    RealVectorGenotype(const std::vector<double>& genes);

    //Copy constructor
    RealVectorGenotype(const RealVectorGenotype& genotype);

    //Constructor to create genome from file
    RealVectorGenotype(const std::string& file_name);

    void print_genotype(std::ofstream& file) override;

protected:

    virtual RealVectorGenotype* clone_impl() const override { return new RealVectorGenotype(*this); };

};

} // namespace Genotypes
} // namespace NeuroEvo

#endif
