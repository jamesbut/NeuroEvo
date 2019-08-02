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
namespace GPMaps {

class DCTMap : public GPMap {

public:

    // C represents the number of coefficients that will be used in the reconstruction
    DCTMap(const unsigned C, const unsigned NUM_NEURONS, const unsigned INPUTS_PER_NEURON);

    // Perform DCTIII (inverse DCTII) on genotype
    Phenotypes::Phenotype* map(Genotypes::Genotype& genotype,
                               Phenotypes::PhenotypeSpec& pheno_spec) override;

    inline std::optional<Utils::Matrix<double>> get_map() override {
        return std::optional<Utils::Matrix<double>>();
    };

    inline void print_gp_map(std::ofstream& file) override {};

protected:

    DCTMap* clone_impl() const override { return new DCTMap(*this); };

private:

    const unsigned _C;
    const unsigned _NUM_NEURONS;
    const unsigned _INPUTS_PER_NEURON;

    void remove_higher_frequencies(Utils::Matrix<double>& coefficients);

};

} // namespace GPMaps
} // namespace NeuroEvo

#endif
