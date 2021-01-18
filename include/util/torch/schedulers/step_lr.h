#ifndef _STEP_LR_
#define _STEP_LR_

#include <util/torch/schedulers/learning_rate_scheduler.h>

/*
    This scheduler multiplies the learning rate by gamma every step size number
    of epochs.
*/

namespace NeuroEvo {

class StepLR : public LearningRateScheduler {

public:

    StepLR(torch::optim::Optimizer& optimiser,
           const unsigned& epoch_num,   //ref to epoch number
           const unsigned step_size,
           const double gamma);

    void step() override;

private:

    const unsigned& _epoch_num;
    const unsigned _step_size;
    const double _gamma;

};

} // namespace NeuroEvo

#endif
