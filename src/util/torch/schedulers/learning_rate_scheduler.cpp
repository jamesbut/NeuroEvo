#include <util/torch/schedulers/learning_rate_scheduler.h>
#include <torch/optim/adam.h>
//#include <torch/optim/sgd.h>

namespace NeuroEvo {

LearningRateScheduler::LearningRateScheduler(torch::optim::Optimizer& optimiser) :
    _param_group_lrs(ascertain_param_group_lr_refs(optimiser)) {}

std::vector<double*> LearningRateScheduler::ascertain_param_group_lr_refs(
        torch::optim::Optimizer& optimiser) const
{

    std::vector<double*> lr_refs;

    auto adam_options = static_cast<torch::optim::AdamOptions*>(
        &optimiser.param_groups().at(0).options());

    //Adam
    if(adam_options)
        for(auto& param_group : optimiser.param_groups())  
            lr_refs.push_back(
                &static_cast<torch::optim::AdamOptions&>(param_group.options()).lr());

    //TODO: Check for failure cast

    return lr_refs;

}

void LearningRateScheduler::set_learning_rate(const double learning_rate)
{
    for(auto& lr : _param_group_lrs)
        *lr = learning_rate;
}

double LearningRateScheduler::get_learning_rate() const
{
    return *_param_group_lrs.at(0);
}

} // namespace NeuroEvo
