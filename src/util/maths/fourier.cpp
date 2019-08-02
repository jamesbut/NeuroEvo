#include <util/maths/fourier.h>
#include <cmath>

namespace NeuroEvo {
namespace Utils {

Matrix<double> DCTII(Matrix<double>& matrix) {

    Matrix<double> coefficients(matrix.get_height(), matrix.get_width());

    for(unsigned i = 0; i < matrix.get_height(); i++) {
        for(unsigned j = 0; j < matrix.get_width(); j++) {

            double ci, cj;

            if(i == 0)
                ci = 1 / sqrt(matrix.get_height());
            else
                ci = sqrt(2) / sqrt(matrix.get_height());

            if(j == 0)
                cj = 1 / sqrt(matrix.get_width());
            else
                cj = sqrt(2) / sqrt(matrix.get_width());

            double sum = 0;
            for(unsigned x = 0; x < matrix.get_height(); x++) {
                for(unsigned y = 0; y < matrix.get_width(); y++) {

                    sum += matrix.at(x, y) *
                           std::cos((2 * x + 1) * i * M_PI / (2 * matrix.get_height())) *
                           std::cos((2 * y + 1) * j * M_PI / (2 * matrix.get_width()));

                }
            }

            double coefficient = ci * cj * sum;
            coefficients.set(i, j, coefficient);

        }
    }

    return coefficients;

}

Matrix<double> DCTIII(Matrix<double>& matrix) {

    Matrix<double> reconstructions(matrix.get_height(), matrix.get_width());

    for(unsigned i = 0; i < matrix.get_height(); i++) {
        for(unsigned j = 0; j < matrix.get_width(); j++) {

            double sum = 0;
            for(unsigned x = 0; x < matrix.get_height(); x++) {
                for(unsigned y = 0; y < matrix.get_width(); y++) {

                    double cx, cy;

                    if(x == 0)
                        cx = 1 / sqrt(matrix.get_height());
                    else
                        cx = sqrt(2) / sqrt(matrix.get_height());

                    if(y == 0)
                        cy = 1 / sqrt(matrix.get_width());
                    else
                        cy = sqrt(2) / sqrt(matrix.get_width());

                    sum += cx * cy * matrix.at(x, y) *
                           std::cos((2 * i + 1) * x * M_PI / (2 * matrix.get_height())) *
                           std::cos((2 * j + 1) * y * M_PI / (2 * matrix.get_width()));

                }
            }

            double reconstruction = sum;
            reconstructions.set(i, j, reconstruction);

        }
    }

    return reconstructions;

}

} // namespace Utils
} // namespace NeuroEvo
