#ifndef _NUMBER_RANGES_H_
#define _NUMBER_RANGES_H_

/*
 * A few functions that return ranges of numbers as vectors or torch tensors
 */

#include <vector>
#include <data/json.h>
#if USE_TORCH
#include <torch/types.h>
#endif

namespace NeuroEvo {

struct Range
{
    Range(const double lower_bound, const double upper_bound, const double increment) :
        lower_bound(lower_bound),
        upper_bound(upper_bound),
        increment(increment) {}

    Range(const JSON& json) :
        lower_bound(json.at({"lower_bound"})),
        upper_bound(json.at({"upper_bound"})),
        increment(json.at({"increment"})) {}

    double lower_bound;
    double upper_bound;
    double increment;
};

//Creates a vector of doubles between lower and upper bound with increment
std::vector<double> create_range(const Range& range);

//Creates a vector of pairs - the pairs are enumerated ranges
std::vector<std::pair<double, double>> create_range_2d(const Range& range);

//Creates range between lower and upper bound with a specified number of values
std::vector<double> create_range_w_size(const double lower_bound,
                                        const double upper_bound,
                                        const unsigned num_values);

#if USE_TORCH
torch::Tensor create_range_torch(const Range& range);

torch::Tensor create_range_torch_2d(const Range& range);
#endif

} // namespace NeuroEvo

#endif
