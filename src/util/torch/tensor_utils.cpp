#include <torch/csrc/autograd/generated/variable_factories.h>
#include <util/torch/tensor_utils.h>

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

double measure_square_symmetry(const torch::Tensor& t)
{
    double total_symmetry = 0.;
    for(int64_t i = 0; i < t.size(0); i++)
    {

        double row_symmetry = 0.;
        for(int64_t j = 0; j < (t.size(1)/2); j++)
        {
            const double v1 = t.index({i, j}).item<double>();
            const double v2 = t.index({i, t.size(1)-j-1}).item<double>();
            //Measure square symmetry
            row_symmetry += std::fabs(pow(v1, 2) - v2);
        }
        total_symmetry += row_symmetry / t.size(1);

    }
    return total_symmetry / t.size(0);
}

torch::Tensor apply_gaussian_noise(const torch::Tensor& t, const double sigma)
{
    return t + torch::normal(0, sigma, t.sizes());
}

} // namespace NeuroEvo
