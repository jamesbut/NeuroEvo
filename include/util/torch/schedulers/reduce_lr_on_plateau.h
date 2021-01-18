#ifndef _REDUCE_LR_ON_PLATEAU_H_
#define _REDUCE_LR_ON_PLATEAU_H_

#include <util/torch/schedulers/learning_rate_scheduler.h>

namespace NeuroEvo {

class ReduceLROnPlateau : public LearningRateScheduler 
{

public:

    ReduceLROnPlateau(torch::optim::Optimizer& optimser,
                      const double& metric,
                      const double factor = 0.1,
                      const unsigned patience = 10);

    void step() override;

private: 

    //Reference to metric being monitored
    const double& _metric;

    //Factor by which to reduce learning rate when metric has plateaued
    const double _factor;

    //Number of epochs with no impriovement after which learning rate will be reduced
    const unsigned _patience;

    unsigned _num_epochs_no_improvement;
    std::optional<double> _lowest_metric_value;

};

} // namespace NeuroEvo
 
#endif
