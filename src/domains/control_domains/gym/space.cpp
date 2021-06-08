#include <domains/control_domains/gym/space.h>

namespace NeuroEvo {

DiscreteSpace::DiscreteSpace(const unsigned n) :
    _n(n) {}

DiscreteSpace* DiscreteSpace::clone_impl() const
{
    return new DiscreteSpace(*this);
}

unsigned DiscreteSpace::get_num_elements() const
{
    return _n;
}

void DiscreteSpace::print() const
{
    std::cout << "n: " << _n << std::endl;
}




BoxSpace::BoxSpace(const std::vector<unsigned> shape,
                   const double low, const double high) :
    _shape(shape), _low(low), _high(high) {}

BoxSpace* BoxSpace::clone_impl() const
{
    return new BoxSpace(*this);
}

unsigned BoxSpace::get_num_elements() const
{
    if(_shape.empty())
        return 0;
    else
    {
        unsigned num_elements = 1;
        for(const auto& s : _shape)
            num_elements *= s;

        return num_elements;
    }
}

double BoxSpace::get_low() const
{
    return _low;
}

double BoxSpace::get_high() const
{
    return _high;
}

void BoxSpace::print() const
{
    std::cout << "Shape: ( ";
    for(const auto& s : _shape)
        std::cout << s << " ";
    std::cout << ")   ";

    std::cout << "Low: " << _low << "   ";
    std::cout << "High: " << _high << "   " << std::endl;
}

} // namespace NeuroEvo
