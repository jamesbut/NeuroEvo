#include <util/algorithms/gan.h>
#include <genotype/genotype_spec.h>

namespace NeuroEvo {

GAN::GAN(const Matrix<double>& true_data, NetworkBuilder& generator_builder,
         NetworkBuilder& discriminator_builder, 
         std::unique_ptr<Distribution<double>> init_net_weight_distr) :
    _true_data(true_data),
    _generator(build_network(generator_builder, init_net_weight_distr)),
    _discriminator(build_network(discriminator_builder, init_net_weight_distr))
{

}

//Build network depending on whether an initial weight distribution has been defined
TorchNetwork* GAN::build_network(NetworkBuilder& net_builder, 
                                 std::unique_ptr<Distribution<double>>& init_net_weight_distr)
{
    
    std::unique_ptr<Genotype<double>> init_genotype(new Genotype<double>());

    if(init_net_weight_distr)
    {
        GenotypeSpec<double> genotype_spec(net_builder.get_num_params(), 
                                           *init_net_weight_distr);
        init_genotype.reset(genotype_spec.generate_genotype());
    } 
        
    auto torch_net = net_builder.generate_phenotype(*init_genotype, nullptr);
    return dynamic_cast<TorchNetwork*>(torch_net); 

}

} // namespace NeuroEvo
