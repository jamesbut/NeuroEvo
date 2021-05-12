#ifndef _ACROBOT_H_
#define _ACROBOT_H_

/*
 * Acrobot using OpenAI gym
 */

#include <domains/control_domains/gym/gym_domain.h>

namespace NeuroEvo {

template <typename G>
class Acrobot : public GymDomain<G>
{

public:

    Acrobot(const bool render = false, const bool domain_trace = false,
            const std::optional<const unsigned> seed = std::nullopt) :
        GymDomain<G>("Acrobot-v1", -100., render, domain_trace, seed) {}

private:

    Acrobot<G>* clone_impl() const override
    {
        return new Acrobot<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
