#include <util/torch/schedulers/step_lr.h>

namespace NeuroEvo {

StepLR::StepLR(torch::optim::Optimizer& optimiser,
               const unsigned& epoch_num,
               const unsigned step_size,
               const double gamma) :
    LearningRateScheduler(optimiser),
    _epoch_num(epoch_num),
    _step_size(step_size),
    _gamma(gamma) {}

void StepLR::step() 
{

    if(_epoch_num == 0)
        std::cout << "StepLR scheduler: Learning rate: " << get_learning_rate() << std::endl;

    if(((_epoch_num+1) % _step_size) == 0)
    {
        set_learning_rate(get_learning_rate() * _gamma);
        std::cout << "StepLR scheduler: Changing learning rate to: " << get_learning_rate() 
            << std::endl;

    }
     
}

} // namespace NeuroEvo
