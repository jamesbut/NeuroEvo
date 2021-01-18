#ifndef _LEARNING_RATE_SCHEDULER_H_
#define _LEARNING_RATE_SCHEDULER_H_

#include <torch/optim/optimizer.h>

namespace NeuroEvo {

class LearningRateScheduler {

public:

    LearningRateScheduler(torch::optim::Optimizer& optimiser);

    //To be implemented: how the scheduler alters the learning rate
    virtual void step() = 0;

    //There is an issue - even though all optimiser options are derived from a single
    //base class, the base class does not contain the learning rate, dispite it being
    //common in all optimisers :/
    //So in order to get pointers to the learning rates(s) (of the parameter 'groups')
    //we have to try to cast to each optimiser option in this function and then get 
    //the pointers to said learning rates.
    //With these pointers we should be able to modify at will.
    std::vector<double*> ascertain_param_group_lr_refs(torch::optim::Optimizer& optimiser) 
        const;

protected:

    //Iterates across all the param group learning rates
    //They will always be set to the same value
    void set_learning_rate(const double learning_rate);
    double get_learning_rate() const;

private:

    //Pointers to the learning rates of each of the different parameter groups
    std::vector<double*> _param_group_lrs; 

};


} // namespace NeuroEvo

#endif
