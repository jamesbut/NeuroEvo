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

//Converts vector of vector of doubles to torch tensor
torch::Tensor matrix_to_tensor(std::vector<std::vector<double>>& matrix);

//Converts torch tensor to vector of vector of doubles
std::vector<std::vector<double>> tensor_to_matrix(const torch::Tensor& t);

} // namespace NeuroEvo

#endif
