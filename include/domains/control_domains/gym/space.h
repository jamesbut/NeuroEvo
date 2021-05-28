#ifndef _SPACE_H_
#define _SPACE_H_

/*
 * A way to define gym observation and action spaces
 */

#include <vector>
#include <iostream>

namespace NeuroEvo {

struct Space
{
    virtual ~Space() = default;

    auto clone() const
    {
        return std::unique_ptr<Space>(clone_impl());
    }

    virtual unsigned get_num_elements() const = 0;
    virtual void print() const = 0;

protected:

    virtual Space* clone_impl() const = 0;

};

struct DiscreteSpace : Space
{
    DiscreteSpace(const unsigned n) :
        _n(n) {}

private:

    DiscreteSpace* clone_impl() const override
    {
        return new DiscreteSpace(*this);
    }

    unsigned get_num_elements() const override
    {
        return _n;
    }

    void print() const override
    {
        std::cout << "n: " << _n << std::endl;
    }

    const unsigned _n;
};

struct BoxSpace : Space
{
    BoxSpace(const std::vector<unsigned> shape, const double low,
             const double high) :
        _shape(shape), _low(low), _high(high) {}

private:

    BoxSpace* clone_impl() const override
    {
        return new BoxSpace(*this);
    }

    unsigned get_num_elements() const override
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

    void print() const override
    {
        std::cout << "Shape: ( ";
        for(const auto& s : _shape)
            std::cout << s << " ";
        std::cout << ")   ";

        std::cout << "Low: " << _low << "   ";
        std::cout << "High: " << _high << "   " << std::endl;
    }

    const std::vector<unsigned> _shape;
    const double _low;
    const double _high;
};

enum SpaceType
{
    Discrete,
    Box
};

} // namespace NeuroEvo

#endif
