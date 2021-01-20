#include <util/torch/schedulers/per_epoch_lr.h>

namespace NeuroEvo {

PerEpochLR::PerEpochLR(torch::optim::Optimizer& optimiser,
                       const unsigned& epoch_num,
                       const std::map<unsigned, double>& epoch_lr_map) :
    LearningRateScheduler(optimiser),
    _epoch_num(epoch_num),
    _epoch_lr_map(epoch_lr_map) {}

void PerEpochLR::step()
{
    
    const auto epoch_iter = _epoch_lr_map.find(_epoch_num);
    if(epoch_iter != _epoch_lr_map.end())
    {
        std::cout << "PerEpochLR scheduler: Changing learning rate to: " 
            << epoch_iter->second << std::endl;
        set_learning_rate(epoch_iter->second);
    }

}

} // namespace NeuroEvo
