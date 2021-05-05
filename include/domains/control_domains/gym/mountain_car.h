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

    MountainCar<G>* clone_impl() const override
    {
        return new MountainCar<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
