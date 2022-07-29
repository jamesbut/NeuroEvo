#include <util/statistics/distributions/gaussian_distribution.h>
#include <util/factory.h>

namespace NeuroEvo {

GaussianDistribution::GaussianDistribution(
    const double mean,
    const double stddev,
    const std::optional<unsigned> seed) :
    Distribution(seed),
    _distr(mean, stddev) {}

GaussianDistribution::GaussianDistribution(const JSON& config) :
    GaussianDistribution(
        config.get<const double>({"mean"}),
        config.get<const double>({"stddev"})
    ) {}

double GaussianDistribution::next()
{
    return _distr(_rng);
}

void GaussianDistribution::reset()
{
    _distr.reset();
}

GaussianDistribution* GaussianDistribution::clone_impl() const
{
    return new GaussianDistribution(*this);
}

JSON GaussianDistribution::to_json_impl() const
{
    JSON json;
    json.emplace("name", "GaussianDistribution");
    json.emplace("mean", _distr.mean());
    json.emplace("stddev", _distr.stddev());
    return json;
}

static Factory<Distribution<double>>::Registrar 
    gaussian_distribution_registrar(
        "GaussianDistribution",
        [](const JSON& config)
        {return std::make_shared<GaussianDistribution>(config);}
    );

} // namespace NeuroEvo
