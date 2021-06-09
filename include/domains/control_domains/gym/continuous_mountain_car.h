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
                          const bool render = false, const bool domain_trace = false,
                          const std::optional<const unsigned> seed = std::nullopt) :
        //GymDomain<G>("MountainCarContinuous-v0", SpaceType::Box, kwargs, 90., render,
        GymDomain<G>("MountainCarContinuous-v0", SpaceType::Box, kwargs, 150., render,
                     domain_trace, seed) {}

private:

    ContinuousMountainCar<G>* clone_impl() const override
    {
        return new ContinuousMountainCar<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
