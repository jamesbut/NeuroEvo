#ifndef _GAN_H_
#define _GAN_H_

/*
 * This class defines a Generative Adversarial Network using the PyTorch C++ API
 */

#include <util/maths/matrix.h>

namespace NeuroEvo {

class GAN
{

public:

    GAN(const Matrix<double>& true_data);

private:

    const Matrix<double> _true_data;

};

} // namespace NeuroEvo

#endif
