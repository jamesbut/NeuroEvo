#include <util/statistics/distributions/uniform_real_distribution.h>
#include <util/factory.h>

namespace NeuroEvo {

UniformRealDistribution::UniformRealDistribution(const double lower_bound,
                                                 const double upper_bound,
                                                 const std::optional<unsigned> seed) :
    Distribution(seed),
    _distr(lower_bound, upper_bound) {}

UniformRealDistribution::UniformRealDistribution(const JSON& json) :
    UniformRealDistribution(json.at({"lower_bound"}), json.at({"upper_bound"}),
                            json.value<const std::optional<unsigned>>(
                                {"seed"}, std::nullopt)
                            ) {}

double UniformRealDistribution::next()
{
    return _distr(_rng);
}

double UniformRealDistribution::get(const double lower_bound, const double upper_bound,
                                    const std::optional<unsigned> seed)
{
    UniformRealDistribution distr(lower_bound, upper_bound, seed);
    return distr.next();
}

void UniformRealDistribution::reset()
{
    _distr.reset();
}

UniformRealDistribution* UniformRealDistribution::clone_impl() const
{
    return new UniformRealDistribution(*this);
}

JSON UniformRealDistribution::to_json_impl() const
{
    JSON json;
    json.emplace("name", "UniformRealDistribution");
    json.emplace("lower_bound", _distr.min());
    json.emplace("upper_bound", _distr.max());
    return json;
}

static Factory<Distribution<double>>::Registrar uniform_real_distribution_registrar(
    "UniformRealDistribution",
    [](const JSON& json) {return std::make_shared<UniformRealDistribution>(json);});

} // namespace NeuroEvo
