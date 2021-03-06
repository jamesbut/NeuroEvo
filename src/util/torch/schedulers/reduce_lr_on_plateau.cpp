#include <util/torch/schedulers/reduce_lr_on_plateau.h>

namespace NeuroEvo {

ReduceLROnPlateau::ReduceLROnPlateau(torch::optim::Optimizer& optimiser,
                                     const double& metric,
                                     const double factor,
                                     const unsigned patience,
                                     const std::optional<double>& min_learning_rate) :
    LearningRateScheduler(optimiser),
    _metric(metric),
    _factor(factor),
    _patience(patience),
    _min_learning_rate(min_learning_rate),
    _num_epochs_no_improvement(0),
    _lowest_metric_value(std::nullopt) {}

void ReduceLROnPlateau::step()
{

    //Update lowest metric value
    if(_lowest_metric_value.has_value())
        if(_metric < _lowest_metric_value)
        {
            _lowest_metric_value = _metric;
            _num_epochs_no_improvement = 0;
        } 
        else
            _num_epochs_no_improvement++;
    else
        _lowest_metric_value = _metric;

    //std::cout << "Lowest metric value: " << _lowest_metric_value.value() << std::endl;
    //std::cout << "Num epochs no improvement: " << _num_epochs_no_improvement << std::endl;

    //Check for plateau
    if(_num_epochs_no_improvement >= _patience)
    {
        //Calculate new learning rate
        double new_lr = get_learning_rate() * _factor;

        //Check it is not below minimum learning rate
        if(_min_learning_rate.has_value())
            if(new_lr < _min_learning_rate.value())
                new_lr = _min_learning_rate.value();

        set_learning_rate(new_lr); 

        _num_epochs_no_improvement = 0;
        _lowest_metric_value = _metric;

        std::cout << "ReduceLROnPlateau scheduler: Changing learning rate to: " 
            << get_learning_rate() << std::endl;
    }

}

} // namespace NeuroEvo
