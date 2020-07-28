#include <gp_map/network_maps/double_to_double_net_map.h>
#include <phenotype/vector_phenotype.h>

namespace NeuroEvo {

DoubleToDoubleNetMap::DoubleToDoubleNetMap(NetworkBuilder& net_builder,
                                           PhenotypeSpec* pheno_spec) :
    NetworkMap<double, double>(net_builder, pheno_spec) {}

Phenotype<double>* DoubleToDoubleNetMap::map(Genotype<double>& genotype)
{
    //Push genotype through decoder
    const std::vector<double> decoder_output = _decoder->activate(genotype.genes());

    //The output of the decoder are the traits of the returned vector
    return new VectorPhenotype<double>(decoder_output);
}

DoubleToDoubleNetMap* DoubleToDoubleNetMap::clone_impl() const
{
    return new DoubleToDoubleNetMap(*this);
}

} // namespace NeuroEvo
