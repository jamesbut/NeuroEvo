#include <util/statistics/statistic.h>
#include <numeric>

namespace NeuroEvo {

double calculate_mean(const std::vector<double>& v)
{
    return std::accumulate(v.begin(), v.end(), 0.) / v.size();
}

double calculate_median(const std::vector<double>& v)
{
    return calculate_quantile(v, 0.5);
}

//Linearly interpolate between the values straddling the quantile
double calculate_quantile(const std::vector<double>& v, const double quantile)
{
    //Sort vector
    std::vector<double> sorted_vec = v;
    std::sort(sorted_vec.begin(), sorted_vec.end());

    const double index_q = (sorted_vec.size() - 1) * quantile;
    const std::size_t index_low = floor(index_q);
    const std::size_t index_high = ceil(index_q);
    const double f = index_q - (double)index_low;

    return (1. - f) * sorted_vec[index_low] + f * sorted_vec[index_high];
}

} // namespace NeuroEvo
