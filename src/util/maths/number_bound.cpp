#include <util/maths/number_bound.h>

namespace NeuroEvo {

NumberBound::NumberBound(const double number) :
    _number(number),
    _bound_classification(BoundClassification::value) {}

NumberBound NumberBound::infinity()
{
    return NumberBound(BoundClassification::inf);
}

NumberBound NumberBound::negative_inifinity()
{
    return NumberBound(BoundClassification::neg_inf);
}

NumberBound::NumberBound(const BoundClassification bound_classification) :
    _number(std::nullopt),
    _bound_classification(bound_classification) {}

bool NumberBound::has_value() const
{
    return _number.has_value();
}

double NumberBound::get_value() const
{
    return _number.value();
}

} // namespace NeuroEvo
