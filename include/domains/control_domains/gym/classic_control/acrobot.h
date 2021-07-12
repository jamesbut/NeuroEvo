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

    Acrobot(const std::optional<const GymMakeKwargs>& kwargs = std::nullopt,
            const bool render = false, const bool domain_trace = false,
            const std::optional<const unsigned> seed = std::nullopt) :
        GymDomain<G>("Acrobot-v1", SpaceType::Discrete, kwargs, -100., render,
                     domain_trace, seed) {}

private:

    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override
    {
        this->seed_env();
    }

    Acrobot<G>* clone_impl() const override
    {
        return new Acrobot<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
