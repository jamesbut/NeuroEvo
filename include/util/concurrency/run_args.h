#ifndef _RUN_ARGS_H_
#define _RUN_ARGS_H_

#include <genetic_operators/selection/selection.h>
#include <domains/domain.h>
#include <genotype/genotype_spec.h>

namespace NeuroEvo {

template <typename G, typename T>
struct RunArguments 
{
    unsigned pop_size;
    unsigned max_gens;
    Selection<G, T>* selector;
    Domain<G, T>* domain;
    GenotypeSpec<G>* geno_spec;
    GPMap<G, T>* gp_map;
    std::optional<const std::string> exp_dir_path;
    unsigned& num_winners;
    bool trace = true;
    unsigned num_trials = 1;
    bool domain_parallel = false;
};

} // namespace NeuroEvo

#endif
