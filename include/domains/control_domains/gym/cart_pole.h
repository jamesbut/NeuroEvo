#ifndef _CART_POLE_H_
#define _CART_POLE_H_

/*
 * CartPole using OpenAI gym
 */

#include <domains/control_domains/gym/gym_domain.h>

namespace NeuroEvo {

template <typename G>
class CartPole : public GymDomain<G>
{

public:

    CartPole(const std::optional<const GymMakeKwargs>& kwargs,
             const bool render = false, const bool domain_trace = false,
             const std::optional<const unsigned> seed = std::nullopt) :
        GymDomain<G>("CartPole-v0", kwargs, 200., render, domain_trace, seed) {}

private:

    CartPole<G>* clone_impl() const override
    {
        return new CartPole<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
