#ifndef _GENOME_H_
#define _GENOME_H_

/*
    This is the standard genotype for now and is a
    vector of real numbers.
*/

#include <genotype/genotype.h>

class RealVectorGenotype : public Genotype {

public:

    RealVectorGenotype(const unsigned int NUM_GENES);

    //Copy constructor
    RealVectorGenotype(const RealVectorGenotype& genotype);

    //Constructor to create genome from file
    RealVectorGenotype(const std::string& file_name);

    void print_genotype(std::ofstream& file) override;

protected:

    virtual RealVectorGenotype* clone_impl() const override { return new RealVectorGenotype(*this); };

};

#endif
