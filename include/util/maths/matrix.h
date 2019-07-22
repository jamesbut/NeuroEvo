#ifndef _MATRIX_H_
#define _MATRIX_H_

/*
    A simple matrix template to make matrix
    operations easier to deal with.
*/

template <class T>
class Matrix {

public:

    Matrix(const unsigned height, const unsigned width) :
        _HEIGHT(height),
        _WIDTH(width),
        _matrix(height * width, 0) {}

    Matrix(const std::vector<T>& vector) :
        _HEIGHT(vector.size()),
        _WIDTH(1),
        _matrix(vector) {}

    Matrix(const unsigned height, const unsigned width,
           const std::vector<T>& vector) :
        _HEIGHT(height),
        _WIDTH(width),
        _matrix(vector) {}

    Matrix operator*(const Matrix& m) {

        //Check for valid matrix multiplication
        if(this->_WIDTH != m._HEIGHT) {
            std::string err_msg = "Matrices of size " + std::to_string(this->_HEIGHT) + "x" +
                                  std::to_string(this->_WIDTH) + " and " +
                                  std::to_string(m._HEIGHT) + "x" +
                                  std::to_string(m._WIDTH) + " cannot be multiplied!";
            std::cout << err_msg << std::endl;
            throw err_msg;

        }

        Matrix matrix(this->_HEIGHT, m._WIDTH);

        for(unsigned i = 0; i < this->_HEIGHT; i++)
            for(unsigned j = 0; j < m._WIDTH; j++) {
                T sum = 0;
                for (unsigned k = 0; k < this->_WIDTH; k++)
                    sum += this->_matrix.at(i * this->_WIDTH + k) * m._matrix.at(k *
                           m._WIDTH + j);
                matrix._matrix.at(i * matrix._WIDTH + j) = sum;
            }

        return matrix;

    }


    inline const unsigned get_height() const { return _HEIGHT; };
    inline const unsigned get_width() const { return _WIDTH; };

    inline std::vector<T>& get_vector() { return _matrix; };

    inline const T& at(const unsigned row, const unsigned column) const {

        return _matrix.at(row * _WIDTH + column);

    };

    inline void set(const unsigned row, const unsigned column, const T value) {

        _matrix.at(row * _WIDTH + column) = value;

    };


private:

    const unsigned _HEIGHT;
    const unsigned _WIDTH;

    std::vector<T> _matrix;

};

template <class T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) {

    for(unsigned i = 0; i < m.get_height(); i++) {

        for(unsigned j = 0; j < m.get_width(); j++)
            os << m.at(i, j) << " ";

        os << std::endl;

    }

    return os;

}


#endif
