#include <util/statistics/distributions/uniform_unsigned_distribution.h>

namespace NeuroEvo {

UniformUnsignedDistribution::UniformUnsignedDistribution(
    const std::optional<unsigned> seed) :
    Distribution<unsigned long>(seed),
    _distr(std::numeric_limits<unsigned long>::lowest(),
           std::numeric_limits<unsigned long>::max()) {}

UniformUnsignedDistribution::UniformUnsignedDistribution(
    const unsigned long lower_bound,
    const unsigned long upper_bound,
    const std::optional<unsigned> seed) :
    Distribution<unsigned long>(seed),
    _distr(lower_bound, upper_bound) {}

unsigned long UniformUnsignedDistribution::next()
{
    return _distr(_rng);
}

unsigned long UniformUnsignedDistribution::get(const unsigned long lower_bound,
                                               const unsigned long upper_bound)
{
    UniformUnsignedDistribution distr(lower_bound, upper_bound);
    return distr.next();
}

void UniformUnsignedDistribution::reset()
{
    _distr.reset();
}

UniformUnsignedDistribution* UniformUnsignedDistribution::clone_impl() const
{
    return new UniformUnsignedDistribution(*this);
}

JSON UniformUnsignedDistribution::to_json_impl() const
{
    JSON json;
    json.emplace("name", "UniformUnsignedDistribution");
    json.emplace("lower_bound", _distr.min());
    json.emplace("upper_bound", _distr.max());
    return json;
}

}
