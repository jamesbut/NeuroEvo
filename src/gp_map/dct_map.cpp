#include <gp_map/dct_map.h>
#include <util/maths/fourier.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <phenotype/neural_network/network.h>
#include <cmath>

namespace NeuroEvo {

DCTMap::DCTMap(const unsigned c, const unsigned num_neurons, const unsigned inputs_per_neuron) :
    _c(c),
    _num_neurons(num_neurons),
    _inputs_per_neuron(inputs_per_neuron) {}

Phenotype* DCTMap::map(Genotype<double>& genotype,
                       PhenotypeSpec<double>& pheno_spec) 
{

    // Assume genes are the DCT coefficients
    Utils::Matrix<double> coefficients(_num_neurons, _inputs_per_neuron, genotype.genes());

    // Only keep _C coefficients
    remove_higher_frequencies(coefficients);

    // Use inverse DCT
    Utils::Matrix<double> traits = Utils::DCTIII(coefficients);

    //TODO: For now, can only turn into Fixed Network
    NetworkBuilder<double>* net_spec = dynamic_cast<NetworkBuilder<double>*>(&pheno_spec);
    return new Network(traits.get_vector(), *net_spec);

}

void DCTMap::remove_higher_frequencies(Utils::Matrix<double>& coefficients) 
{

    const unsigned& height = coefficients.get_height();
    const unsigned& width = coefficients.get_width();

    unsigned c = 1;

    // Only _C of the lowest frequencies are kept in the coefficients matrix.
    // The ordering of the frequencies is assumed to be the same as in:
    // Koutnik, Gomez & Schmidhuber (2010)

    // Iterate through matrix in frequency order (lowest to highest).
    // Set all coeffiecient to 0 if the frequency index is greater than _C.
    for(unsigned i = 0; i < (height+width-1); i++) {

        int start_col_index = std::max<int>(0, (i+1-height));
        int start_row_index = std::max<int>(0, (i+1-width));
        int end_row_index = std::min(i, (height-1));
        int end_col_index = std::min(i, (width-1));

        unsigned diagonal_size = std::min(std::min((i+1), height), (width - start_col_index));

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

            if(c > _c)
                coefficients.set(row_index, column_index, 0);
            c++;

        }

    }

}

} // namespace NeuroEvo
