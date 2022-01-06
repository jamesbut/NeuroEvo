#include <torch/csrc/autograd/generated/variable_factories.h>
#include <util/torch/tensor_utils.h>

namespace NeuroEvo {

torch::Tensor matrix_to_tensor(const Matrix<double>& matrix)
{
    if(matrix.get_height() < 1)
        throw std::length_error("Trying to convert matrix to torch tensor that does "
            "not have any elements");

    torch::Tensor t = torch::zeros({(int64_t)matrix.get_height(),
                                    (int64_t)matrix.get_width()});

    for(std::size_t i = 0; i < matrix.get_height(); i++)
        for(std::size_t j = 0; j < matrix.get_width(); j++)
            t.index_put_({(int64_t)i, (int64_t)j}, matrix.at(i,j));

    return t;
}

torch::Tensor vector_to_tensor(const std::vector<double>& vec,
                               torch::IntArrayRef tensor_size)
{
    torch::Tensor tensor = torch::tensor(vec);
    return tensor.reshape(tensor_size);
}

Matrix<double> tensor_to_matrix(const torch::Tensor& t)
{
    //Check for 1d tensor
    if(t.sizes().size() == 1)
    {
        std::vector<double> v(t.size(0));
        for(int64_t i = 0; i < v.size(); i++)
            v[i] = t.index({i}).item<double>();

        return v;

    //2d tensor
    } else
    {

        std::vector<std::vector<double>> v(t.size(0), std::vector<double>(t.size(1), 0.));

        for(unsigned i = 0; i < v.size(); i++)
            for(unsigned j = 0; j < v[i].size(); j++)
                v[i][j] = t.index({(int64_t)i, (int64_t)j}).item<double>();

        return v;

    }
}

std::vector<double> tensor_to_vector(const torch::Tensor& t)
{
    return tensor_to_matrix(t).get_vector();
}

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
