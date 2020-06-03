#ifndef _DCT_MAP_H_
#define _DCT_MAP_H_

/*
    This mapping uses an inverse DCT transform to
    create a phenotype from a genotype. The genotype
    is therefore assumed to be the cosine frequency
    coefficients. One can select the number of coefficients
    to use in the reconstruction via C in the constructor.
*/

#include <gp_map/gp_map.h>

namespace NeuroEvo {

//Assume that DCTMap can only use genes of type double
class DCTMap : public GPMap<double, double> 
{

public:

    // c represents the number of coefficients that will be used in the reconstruction
    DCTMap(const unsigned c, const unsigned num_neurons, const unsigned inputs_per_neuron);

    // Perform DCTIII (inverse DCTII) on genotype
    Phenotype<double>* map(Genotype<double>& genotype,
                           PhenotypeSpec<double, double>& pheno_spec) override;

    std::optional<Utils::Matrix<double>> get_map() override 
    {
        return std::optional<Utils::Matrix<double>>();
    };

    void print_gp_map(std::ofstream& file) override {};

protected:

    DCTMap* clone_impl() const override { return new DCTMap(*this); };

private:

    const unsigned _c;
    const unsigned _num_neurons;
    const unsigned _inputs_per_neuron;

    void remove_higher_frequencies(Utils::Matrix<double>& coefficients);

};

} // namespace NeuroEvo

#endif
