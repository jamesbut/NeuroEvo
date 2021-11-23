#ifndef _RUN_ARGS_H_
#define _RUN_ARGS_H_

#include <domains/domain.h>
#include <optimiser/optimiser.h>

namespace NeuroEvo {

template <typename G, typename T>
struct RunArguments
{
    std::shared_ptr<Domain<G, T>> domain;
    std::shared_ptr<Optimiser<G, T>> optimiser;
    std::shared_ptr<GPMap<G, T>> gp_map;
    std::optional<const std::string> exp_dir_path;
    bool dump_winners_only;
    unsigned& num_winners;
    unsigned& total_winners_gens;
    bool trace = true;
    bool domain_parallel = false;
};

} // namespace NeuroEvo

#endif
