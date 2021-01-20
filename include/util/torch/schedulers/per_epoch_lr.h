#ifndef _PER_EPOCH_LR_H_
#define _PER_EPOCH_LR_H_

#include <util/torch/schedulers/learning_rate_scheduler.h>

/*
    This is a learning rate scheduler where one can literally define a map
    from epoch numbers to learning rates. It is quite hacky but it is useful
    to have.
*/

namespace NeuroEvo {

class PerEpochLR : public LearningRateScheduler {

public:

    PerEpochLR(torch::optim::Optimizer& optimiser,
               const unsigned& epoch_num,
               const std::map<unsigned, double>& epoch_lr_map);

    void step() override;

private:

    const unsigned& _epoch_num;

    //A ordered map from epochs to learning rates
    const std::map<unsigned, double>& _epoch_lr_map;

};

} // namespace NeuroEvo

#endif
