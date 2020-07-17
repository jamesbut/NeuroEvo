#ifndef _NUMBER_RANGES_H_
#define _NUMBER_RANGES_H_

/*
 * A few functions that return ranges of numbers as vectors or torch tensors
 */

#include <vector>
#include <torch/types.h>

namespace NeuroEvo {

//Creates a vector of doubles between lower and upper bound with increment
std::vector<double> create_range(const double lower_bound, const double upper_bound,
                                 const double increment);

//Creates a vector of pairs - the pairs are enumerated ranges
std::vector<std::pair<double, double>> create_range_2d(const double lower_bound,
                                                       const double upper_bound,
                                                       const double increment);

//Creates range between lower and upper bound with a specified number of values
std::vector<double> create_range_w_size(const double lower_bound, const double upper_bound,
                                        const unsigned num_values);

torch::Tensor create_range_torch(const double lower_bound, const double upper_bound,
                                 const double increment);

torch::Tensor create_range_torch_2d(const double lower_bound, const double upper_bound,
                                    const double increment);

} // namespace NeuroEvo

#endif
