#include <util/algorithms/gan.h>

//#include <torch/types.h>

namespace NeuroEvo {

GAN::GAN(const Matrix<double>& true_data) :
    _true_data(true_data) {

        std::cout << "True data:" << std::endl;
        std::cout << _true_data << std::endl;

        //torch::Tensor tensor = torch::eye(3);
        //std::cout << tensor << std::endl;

    }

} // namespace NeuroEvo
