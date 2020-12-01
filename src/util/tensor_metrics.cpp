#include <util/tensor_metrics.h>

namespace NeuroEvo {

double measure_symmetry(const torch::Tensor& t)
{
    double total_symmetry = 0.;
    for(int64_t i = 0; i < t.size(0); i++)
    {

        double row_symmetry = 0.;
        for(int64_t j = 0; j < (t.size(1)/2); j++)
        {
            const double v1 = t.index({i, j}).item<double>();
            const double v2 = t.index({i, t.size(1)-j-1}).item<double>();
            row_symmetry += std::fabs(v1 - v2);
        }
        total_symmetry += row_symmetry / t.size(1);

    }
    return total_symmetry / t.size(0);
}

} // namespace NeuroEvo
