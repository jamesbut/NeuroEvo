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

torch::Tensor matrix_to_tensor(std::vector<std::vector<double>>& matrix)
{
    if(matrix.size() < 1)
        throw std::length_error("Trying to convert matrix to torch tensor that does "
            "not have any elements");

    torch::Tensor t = torch::zeros({(int64_t)matrix.size(),
                                    (int64_t)matrix.at(0).size()});

    for(std::size_t i = 0; i < matrix.size(); i++)
        for(std::size_t j = 0; j < matrix[i].size(); j++)
            t.index_put_({(int64_t)i, (int64_t)j}, matrix[i][j]);

    return t;
}

std::vector<std::vector<double>> tensor_to_matrix(const torch::Tensor& t)
{
    std::vector<std::vector<double>> v(t.size(0), std::vector<double>(t.size(1), 0.));

    for(unsigned i = 0; i < v.size(); i++)
        for(unsigned j = 0; j < v[i].size(); j++)
            v[i][j] = t.index({(int64_t)i, (int64_t)j}).item<double>();

    return v;
}

} // namespace NeuroEvo
