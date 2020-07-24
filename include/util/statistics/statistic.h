#ifndef _STATISTIC_H_
#define _STATISTIC_H_

/*
 * This collection of functions calculates statistics over vectors of values
 */

#include <vector>

namespace NeuroEvo {

double calculate_mean(const std::vector<double>& v);
double calculate_median(const std::vector<double>& v);

double calculate_quantile(const std::vector<double>& v, const double quantile);

} // namespace NeuroEvo

#endif

