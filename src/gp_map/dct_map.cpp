#include <gp_map/dct_map.h>
#include <util/maths/fourier.h>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <phenotype/fixed_network/network.h>

namespace NeuroEvo {
namespace GPMaps {

DCTMap::DCTMap(const unsigned C, const unsigned NUM_NEURONS, const unsigned INPUTS_PER_NEURON) :
    _C(C),
    _NUM_NEURONS(NUM_NEURONS),
    _INPUTS_PER_NEURON(INPUTS_PER_NEURON) {}

Phenotypes::Phenotype* DCTMap::map(Genotypes::Genotype& genotype,
                                   Phenotypes::PhenotypeSpec& pheno_spec) {

    // Assume genes are the DCT coefficients
    Utils::Matrix<double> coefficients(_NUM_NEURONS, _INPUTS_PER_NEURON, genotype.get_genes());

    // Only keep _C coefficients
    remove_higher_frequencies(coefficients);

    // Use inverse DCT
    Utils::Matrix<double> traits = Utils::DCTIII(coefficients);

    //TODO: For now, can only turn into Fixed Network
    Phenotypes::FixedNetworkSpec* net_spec = dynamic_cast<Phenotypes::FixedNetworkSpec*>(&pheno_spec);
    return new Phenotypes::FixedNetwork(traits.get_vector(), *net_spec);

}

void DCTMap::remove_higher_frequencies(Utils::Matrix<double>& coefficients) {

    const unsigned& HEIGHT = coefficients.get_height();
    const unsigned& WIDTH = coefficients.get_width();

    unsigned c = 1;

    // Only _C of the lowest frequencies are kept in the coefficients matrix.
    // The ordering of the frequencies is assumed to be the same as in:
    // Koutnik, Gomez & Schmidhuber (2010)

    // Iterate through matrix in frequency order (lowest to highest).
    // Set all coeffiecient to 0 if the frequency index is greater than _C.
    for(unsigned i = 0; i < (HEIGHT+WIDTH-1); i++) {

        int start_col_index = std::max<int>(0, (i+1-HEIGHT));
        int start_row_index = std::max<int>(0, (i+1-WIDTH));
        int end_row_index = std::min(i, (HEIGHT-1));
        int end_col_index = std::min(i, (WIDTH-1));

        unsigned diagonal_size = std::min(std::min((i+1), HEIGHT), (WIDTH - start_col_index));

        for(unsigned j = 0; j < diagonal_size; j++) {

            unsigned row_index;
            unsigned column_index = 0;

            if(j % 2 == 0) {

                row_index = start_row_index + j/2;
                column_index = end_col_index - j/2;

            } else {

                row_index = end_row_index - std::floor((double)j/2);
                column_index = start_col_index + std::floor((double)j/2);

            }

            if(c > _C)
                coefficients.set(row_index, column_index, 0);
            c++;

        }

    }

}

} // namespace GPMaps
} // namespace NeuroEvo
