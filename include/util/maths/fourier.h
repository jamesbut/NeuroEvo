#ifndef _FOURIER_H_
#define _FOURIER_H_

#include <util/maths/matrix.h>

namespace NeuroEvo {
namespace Utils {

// Discrete Fourier Transform II (The DCT)
Matrix<double> DCTII(Matrix<double>& matrix);

// Discrete Fourier Transform III (The inverse DCT)
Matrix<double> DCTIII(Matrix<double>& matrix);

} // namespace Utils
} // namespace NeuroEvo

#endif
