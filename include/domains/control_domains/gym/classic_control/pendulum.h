#ifndef _PENDULUM_H_
#define _PENDULUM_H_

#include <domains/control_domains/gym/gym_domain.h>

namespace NeuroEvo {

template <typename G>
class Pendulum : public GymDomain<G>
{

public:

    Pendulum(const std::optional<const GymMakeKwargs>& kwargs = std::nullopt,
             const bool render = false, const bool domain_trace = false,
             const std::optional<const unsigned> seed = std::nullopt) :
        //Reward threshold: 100. for now, but there is nothing given in openai gym code
        GymDomain<G>("Pendulum-v0", SpaceType::Box, kwargs, 100., render,
                     domain_trace, seed) {}

private:

    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override
    {
        this->seed_env();
    }

    Pendulum<G>* clone_impl() const override
    {
        return new Pendulum<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
