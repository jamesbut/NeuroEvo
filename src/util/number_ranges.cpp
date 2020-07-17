#include <util/number_ranges.h>
#include <math.h>

namespace NeuroEvo {

std::vector<double> create_range(const double lower_bound, const double upper_bound,
                                 const double increment)
{
    std::vector<double> number_range;
    double value = lower_bound;
    while(value <= upper_bound)
    {
        number_range.push_back(value);
        value += increment;
    }
    
    return number_range;
}

std::vector<std::pair<double, double>> create_range_2d(const double lower_bound, 
                                                             const double upper_bound, 
                                                             const double increment)
{
    std::vector<std::pair<double, double>> range_pairs;
    const unsigned approx_num_vals = pow(((upper_bound - lower_bound) / increment), 2);
    range_pairs.reserve(approx_num_vals);

    double x_val = lower_bound;
    double y_val = lower_bound;

    while(x_val <= upper_bound)
    {
        while(y_val <= upper_bound)
        {
            range_pairs.push_back(std::pair<double, double>(x_val, y_val));
            y_val += increment;
        }
        y_val = lower_bound;
        x_val += increment;
    }

    return range_pairs;
}

std::vector<double> create_range_w_size(const double lower_bound, const double upper_bound,
                                        const unsigned num_values)
{
    const double range_size = upper_bound - lower_bound;
    const double increment = range_size / num_values;

    return create_range(lower_bound, upper_bound, increment);
}

torch::Tensor create_range_torch(const double lower_bound, const double upper_bound,
                                 const double increment)
{
    std::vector<double> num_range = create_range(lower_bound, upper_bound, increment);

    torch::Tensor torch_range = torch::zeros({(int64_t)num_range.size()});
    for(std::size_t i = 0; i < num_range.size(); i++)
        torch_range.index_put_({(int64_t)i}, num_range[i]);

    return torch_range;
}

torch::Tensor create_range_torch_2d(const double lower_bound, const double upper_bound,
                                    const double increment)
{
    std::vector<double> num_range = create_range(lower_bound, upper_bound, increment);

    torch::Tensor torch_range = torch::zeros({(int64_t)num_range.size(), 1});
    for(std::size_t i = 0; i < num_range.size(); i++)
        torch_range.index_put_({(int64_t)i, 0}, num_range[i]);

    return torch_range;
}

} // namespace NeuroEvo
