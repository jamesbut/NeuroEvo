#ifndef _CONTINUOUS_MOUNTAIN_CAR_H_
#define _CONTINUOUS_MOUNTAIN_CAR_H_

/*
 * The continuous version of MountainCar from OpenAI gym
 */

#include <domains/control_domains/gym/gym_domain.h>

namespace NeuroEvo {

template <typename G>
class ContinuousMountainCar : public GymDomain<G>
{

public:

    ContinuousMountainCar(const std::optional<const GymMakeKwargs>& kwargs,
                          const std::optional<const std::vector<const double>>&
                              power_values = std::nullopt,
                          const bool render = false, const bool domain_trace = false,
                          const std::optional<const unsigned> seed = std::nullopt) :
        //GymDomain<G>("MountainCarContinuous-v0", SpaceType::Box, kwargs, 90., render,
        GymDomain<G>("MountainCarContinuous-v0", SpaceType::Box, kwargs, 2., render,
                     domain_trace, seed),
        _power_values(power_values) {}

private:

    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override
    {
        this->seed_env();

        //Reset domain with different power values
        if(_power_values.has_value())
        {
            std::cout << "Kwargs before:" << std::endl;
            std::cout << this->_kwargs.value();

            //Remove previous power value
            this->_kwargs->remove_kwarg("power");

            //Randomly select new power
            UniformUnsignedDistribution distr(0, _power_values->size()-1);
            const double new_power =
                _power_values.value()[distr.next()];
            //Set new power
            this->_kwargs->set_kwarg("power", new_power);

            std::cout << "Kwargs after:" << std::endl;
            std::cout << this->_kwargs.value();

            this->make_env("MountainCarContinuous-v0", this->_kwargs.value());
        }

    }

    ContinuousMountainCar<G>* clone_impl() const override
    {
        return new ContinuousMountainCar<G>(*this);
    }

    const std::optional<const std::vector<const double>> _power_values;

};

} // namespace NeuroEvo

#endif
