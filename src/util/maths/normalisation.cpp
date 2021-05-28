#include <util/maths/normalisation.h>

namespace NeuroEvo {

double normalise(const double value, const double orig_min, const double orig_max,
                 const double new_min, const double new_max)
{
    const double orig_range = orig_max - orig_min;
    const double new_range = new_max - new_min;
    return new_range * (value - orig_min) / orig_range + new_min;
}

} // namespace NeuroEvo
