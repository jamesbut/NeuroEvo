#include <util/statistics/distributions/bernoulli_double_distribution.h>

namespace NeuroEvo {

BernoulliDoubleDistribution::BernoulliDoubleDistribution(const double p) :
    _distr(p) {}

double BernoulliDoubleDistribution::next()
{
    return static_cast<double>(_distr(_rng));
}

void BernoulliDoubleDistribution::reset()
{
    _distr.reset();
}

BernoulliDoubleDistribution* BernoulliDoubleDistribution::clone_impl() const
{
    return new BernoulliDoubleDistribution(*this);
}

JSON BernoulliDoubleDistribution::to_json_impl() const
{
    JSON json;
    json.emplace("name", "BernoulliDoubleDistribution");
    json.emplace("p", _distr.p());
    return json;
}

} // namespace NeuroEvo
