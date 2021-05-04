#ifndef _MOUNTAIN_CAR_H_
#define _MOUNTAIN_CAR_H_

/*
 * MountainCar uses the OpenAI gym library
 */

#include <domains/control_domains/gym/gym_domain.h>

namespace NeuroEvo {

template <typename G>
class MountainCar : public GymDomain<G>
{

public:

    MountainCar(const bool render = false, const bool domain_trace = false,
                const std::optional<const unsigned> seed = std::nullopt) :
        GymDomain<G>("MountainCar-v0", 0., render, domain_trace, seed) {}

private:


    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override
    {
        const NetworkBuilder* network_builder =
            dynamic_cast<const NetworkBuilder*>(&pheno_spec);

        //If it is not a network
        if(network_builder == nullptr)
        {
            std::cout << "Only network specifications are allowed with" <<
                        " the mountain car domain!" << std::endl;
            return false;
        }

        //TODO: Write this
        //If it has the correct number of inputs and outputs
        /*
        if(network_builder->get_num_inputs() != 2)
        {
            std::cout << "Number of inputs must be 2 " <<
                        "for the non-markovian single part cole domain" <<
                        std::endl;
            return false;
        }

        if((network_builder->get_num_outputs() != 1) &&
           (network_builder->get_num_outputs() != 2))
        {
            std::cerr << "Number of outputs must be 1 or 2 for SCP domain" << std::endl;
            return false;
        }
        */

        return true;

    }

    MountainCar<G>* clone_impl() const override
    {
        return new MountainCar<G>(*this);
    }


};

} // namespace NeuroEvo

#endif
