#ifndef _SPACE_H_
#define _SPACE_H_

/*
 * A way to define gym observation and action spaces
 */

#include <vector>
#include <iostream>
#include <memory>

namespace NeuroEvo {

class Space
{

public:

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

class DiscreteSpace : public Space
{

public:

    DiscreteSpace(const unsigned n);

private:

    DiscreteSpace* clone_impl() const override;

    unsigned get_num_elements() const override;

    void print() const override;

    const unsigned _n;
};

class BoxSpace : public Space
{

public:

    BoxSpace(const std::vector<unsigned> shape, const double low, const double high);

    double get_low() const;
    double get_high() const;

private:

    BoxSpace* clone_impl() const override;

    unsigned get_num_elements() const override;

    void print() const override;

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
