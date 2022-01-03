#include <util/number_ranges.h>
#include <math.h>

namespace NeuroEvo {

std::vector<double> create_range(const Range& range)
{
    std::vector<double> number_range;
    double value = range.lower_bound;
    while(value <= range.upper_bound)
    {
        number_range.push_back(value);
        value += range.increment;
    }

    return number_range;
}

std::vector<std::pair<double, double>> create_range_2d(const Range& range)
{
    std::vector<std::pair<double, double>> range_pairs;
    const unsigned approx_num_vals = pow(((range.upper_bound - range.lower_bound) /
                                          range.increment), 2);
    range_pairs.reserve(approx_num_vals);

    double x_val = range.lower_bound;
    double y_val = range.lower_bound;

    while(x_val <= range.upper_bound)
    {
        while(y_val <= range.upper_bound)
        {
            range_pairs.push_back(std::pair<double, double>(x_val, y_val));
            y_val += range.increment;
        }
        y_val = range.lower_bound;
        x_val += range.increment;
    }

    return range_pairs;
}

std::vector<double> create_range_w_size(const double lower_bound,
                                        const double upper_bound,
                                        const unsigned num_values)
{
    const double range_size = upper_bound - lower_bound;
    const double increment = range_size / num_values;

    return create_range(Range(lower_bound, upper_bound, increment));
}

#if USE_TORCH
torch::Tensor create_range_torch(const Range& range)
{
    std::vector<double> num_range = create_range(range);

    torch::Tensor torch_range = torch::zeros({(int64_t)num_range.size(), 1},
                                             {torch::kFloat64});
    for(int64_t i = 0; i < static_cast<int64_t>(num_range.size()); i++)
        torch_range.index_put_({i, 0}, num_range[i]);

    return torch_range;
}

torch::Tensor create_range_torch_2d(const Range& range)
{
    std::vector<double> num_range = create_range(range);

    torch::Tensor torch_range = torch::zeros({(int64_t)num_range.size(), 1});
    for(std::size_t i = 0; i < num_range.size(); i++)
        torch_range.index_put_({(int64_t)i, 0}, num_range[i]);

    return torch_range;
}
#endif

} // namespace NeuroEvo
