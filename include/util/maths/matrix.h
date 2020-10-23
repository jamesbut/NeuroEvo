#ifndef _MATRIX_H_
#define _MATRIX_H_

/*
    A simple matrix template to make matrix
    operations easier to deal with.
*/

#include <vector>
#include <string>
#include <iostream>

namespace NeuroEvo {

template <class T>
class Matrix 
{

public:

    Matrix(const unsigned height, const unsigned width) :
        _height(height),
        _width(width),
        _matrix(height * width, 0) {}

    Matrix(const std::vector<T>& v) :
        _height(v.size()),
        _width(1),
        _matrix(v) {}

    Matrix(const unsigned height, const unsigned width,
           const std::vector<T>& v) :
        _height(height),
        _width(width),
        _matrix(v) {}

    Matrix(const std::vector<std::vector<T>>& matrix) :
        _height(matrix.size()),
        _width(matrix.at(0).size()),
        _matrix(flatten(matrix)) {}

    Matrix operator*(const Matrix& m) 
    {

        //Check for valid matrix multiplication
        if(this->_width != m._height) 
        {
            std::string err_msg = "Matrices of size " + std::to_string(this->_height) + "x" +
                                  std::to_string(this->_width) + " and " +
                                  std::to_string(m._height) + "x" +
                                  std::to_string(m._width) + " cannot be multiplied!";
            std::cout << err_msg << std::endl;
            throw err_msg;
        }

        Matrix matrix(this->_height, m._width);

        for(unsigned i = 0; i < this->_height; i++)
            for(unsigned j = 0; j < m._width; j++) 
            {
                T sum = 0;
                for (unsigned k = 0; k < this->_width; k++)
                    sum += this->_matrix.at(i * this->_width + k) * m._matrix.at(k *
                           m._width + j);
                matrix._matrix.at(i * matrix._width + j) = sum;
            }

        return matrix;

    }

    inline unsigned get_height() const { return _height; };
    inline unsigned get_width() const { return _width; };

    inline const std::vector<T>& get_vector() const { return _matrix; };

    const T at(const unsigned row, const unsigned column) const
    {
        return _matrix.at(row * _width + column);
    };

    void set(const unsigned row, const unsigned column, const T value) 
    {
        _matrix.at(row * _width + column) = value;
    };

    //Fill matrix with value
    void fill(const T& value)
    {
        for(std::size_t i = 0; i < _matrix.size(); i++)
            _matrix[i] = value;
    }


private:

    const unsigned _height;
    const unsigned _width;

    std::vector<T> _matrix;

    std::vector<T> flatten(const std::vector<std::vector<T>>& matrix) 
    {
        std::vector<T> vec;
        vec.reserve(matrix.size() * matrix.at(0).size());

        for(const auto& row : matrix)
            vec.insert(vec.end(), row.begin(), row.end());
        return vec;
    }

};

template <class T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) 
{

    for(unsigned i = 0; i < m.get_height(); i++) 
    {
        for(unsigned j = 0; j < m.get_width(); j++)
            os << m.at(i, j) << " ";

        if(i != (m.get_height()-1))
            os << std::endl;
    }

    return os;

}

} // namespace NeuroEvo

#endif
