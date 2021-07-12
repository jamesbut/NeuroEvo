#ifndef _BIPEDAL_WALKER_H_
#define _BIPEDAL_WALKER_H_

#include <domains/control_domains/gym/gym_domain.h>

namespace NeuroEvo {

template <typename G>
class BipedalWalker : public GymDomain<G>
{

public:

    BipedalWalker(const std::optional<const GymMakeKwargs>& kwargs = std::nullopt,
                  const bool render = false, const bool domain_trace = false,
                  const std::optional<const unsigned> seed = std::nullopt) :
        GymDomain<G>("BipedalWalker-v3", SpaceType::Box, kwargs, 300., render,
                     domain_trace, seed) {}

private:

    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override
    {
        this->seed_env();
    }

    BipedalWalker<G>* clone_impl() const override
    {
        return new BipedalWalker<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
