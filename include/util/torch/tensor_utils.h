#ifndef _TENSOR_UTILS_H_
#define _TENSOR_UTILS_H_

/*
 * These are just some torch tensor utils
*/

#include <torch/types.h>

namespace NeuroEvo {

//Measure average vertical symmetry of 2d tensor
double measure_symmetry(const torch::Tensor& t);

//Measure average vertical square symmetry of 2d tensor
double measure_square_symmetry(const torch::Tensor& t);

//Apply gaussian noise to copy of torch tensor and return copy
torch::Tensor apply_gaussian_noise(const torch::Tensor& t, const double sigma);

} // namespace NeuroEvo

#endif
