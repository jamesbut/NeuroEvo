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




BoxSpace::BoxSpace(const std::vector<unsigned>& shape,
                   const std::vector<double>& lows,
                   const std::vector<double>& highs) :
    _shape(shape), _lows(lows), _highs(highs) {}

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

std::vector<double> BoxSpace::get_lows() const
{
    return _lows;
}

std::vector<double> BoxSpace::get_highs() const
{
    return _highs;
}

void BoxSpace::print() const
{
    std::cout << "Shape: ( ";
    for(const auto& s : _shape)
        std::cout << s << " ";
    std::cout << ")   ";

    std::cout << "Lows: [";
    for(const auto l : _lows)
        std::cout << l << " ";
    std::cout << "]   ";
    std::cout << "Highs: [";
    for(const auto h : _highs)
        std::cout << h << " ";
    std::cout << "]" << std::endl;
}

} // namespace NeuroEvo
