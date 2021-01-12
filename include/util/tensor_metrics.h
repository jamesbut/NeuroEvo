#ifndef _TENSOR_METRICS_H_
#define _TENSOR_METRICS_H_

/*
    These are just some functions that can calculate metrics on torch tensors 
*/

#include <torch/types.h>

namespace NeuroEvo {

//Measure average vertical symmetry of 2d tensor 
double measure_symmetry(const torch::Tensor& t);

//Measure average vertical square symmetry of 2d tensor 
double measure_square_symmetry(const torch::Tensor& t);

} // namespace NeuroEvo

#endif
