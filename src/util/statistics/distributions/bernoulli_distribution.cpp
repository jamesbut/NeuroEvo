#include <util/statistics/distributions/bernoulli_distribution.h>

namespace NeuroEvo {

BernoulliDistribution::BernoulliDistribution(const double p) :
    _distr(p) {}

bool BernoulliDistribution::next()
{
    return _distr(_rng);
}

void BernoulliDistribution::reset()
{
    _distr.reset();
}

BernoulliDistribution* BernoulliDistribution::clone_impl() const
{
    return new BernoulliDistribution(*this);
}

JSON BernoulliDistribution::to_json_impl() const
{
    JSON json;
    json.emplace("name", "BernoulliDistribution");
    json.emplace("p", _distr.p());
    return json;
}

} // namespace NeuroEvo
