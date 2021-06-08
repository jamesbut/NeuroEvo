#ifndef _NUMBER_BOUND_H_
#define _NUMBER_BOUND_H_

/*
 * This class formalises and deals with number bounds that can include infinity
 * and negative infinity
 */

#include <optional>

namespace NeuroEvo {

class NumberBound
{

private:

    enum BoundClassification
    {
        inf,
        neg_inf,
        value
    };

public:

    NumberBound(const double number);
    static NumberBound infinity();
    static NumberBound negative_inifinity();

    bool has_value() const;
    //Will throw std::bad_optional_access error if _number is not set
    double get_value() const;

private:

    NumberBound(const BoundClassification bound_classification);

    const std::optional<const double> _number;
    const BoundClassification _bound_classification;

};

} // namespace NeuroEvo

#endif
