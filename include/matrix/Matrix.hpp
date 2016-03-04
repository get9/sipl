#pragma once

#ifndef SIPL_MATRIX_MATRIX_H
#define SIPL_MATRIX_MATRIX_H

#include "matrix/Wrappers.hpp"
#include "matrix/Vector.hpp"
#include "matrix/VectorOps.hpp"
#include "matrix/MatrixBase.hpp"

namespace sipl
{

// Initial class for statically-allocated matrices
template <typename Dtype, int32_t Rows, int32_t Cols>
class Matrix : public MatrixBase<Dtype,
                                 Rows,
                                 Cols,
                                 StaticArrayWrapper<Dtype, Rows * Cols>>
{
public:
    Matrix()
    {
        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Rows * Cols>();
        this->dims = {Rows, Cols};
    }

    Matrix(int32_t rows, int32_t cols)
    {
        assert(rows == Rows && cols == Cols && "size mismatch");
        this->nelements_ = rows * cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Rows * Cols>();
        this->dims = {Rows, Cols};
    }

    Matrix(Dtype fill_value)
    {
        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Rows * Cols>(this->nelements_);
        this->dims = {Rows, Cols};
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
    {
        assert(list.size() == Rows && "size mismatch");
        for (const auto l : list) {
            assert(l.size() == Cols && "size mismatch");
        }

        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Rows * Cols>(this->nelements_);
        this->dims = {Rows, Cols};
        int32_t i = 0;
        for (const auto l : list) {
            std::copy(std::begin(l),
                      std::end(l),
                      std::begin(this->data_) + i++ * l.size());
        }
    }

    Matrix(const Matrix& other)
    {
        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Rows * Cols>();
        this->dims = {Rows, Cols};
        std::copy(std::begin(other.data_),
                  std::end(other.data_),
                  std::begin(this->data_));
    }

    Matrix(Matrix&& other)
    {
        this->nelements_ = other.nelements_;
        this->nbytes_ = other.nbytes_;
        this->data_ = std::move(other.data_);
        this->dims = std::move(other.dims);
    }
};

// Specialization of the above for dynamically-allocated matrices.
template <typename Dtype>
class Matrix<Dtype, Dynamic, Dynamic>
    : public MatrixBase<Dtype, Dynamic, Dynamic, DynamicArrayWrapper<Dtype>>
{
public:
    // Need to use 'this' pointer below because templated base class members are
    // not visible in a certain phase of compilation. See here:
    // http://stackoverflow.com/a/6592617
    Matrix(int32_t rows, int32_t cols)
    {
        this->nelements_ = rows * cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
        this->dims = {rows, cols};
    }

    Matrix(int32_t rows, int32_t cols, Dtype fill_value)
    {
        this->nelements_ = rows * cols;
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
        this->dims = {rows, cols};
    }

    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
    {
        int32_t nrows = list.size();
        int32_t ncols = list.begin()->size();
        for (const auto l : list) {
            assert(int32_t(l.size()) == ncols && "size mismatch");
        }

        this->nelements_ = nrows * ncols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
        this->dims = {nrows, ncols};
        int32_t i = 0;
        for (const auto l : list) {
            std::copy(std::begin(l),
                      std::end(l),
                      std::begin(this->data_) + i++ * l.size());
        }
    }

    Matrix(const Matrix& other)
    {
        this->nelements_ = other.nelements_;
        this->nbytes_ = other.nbytes_;
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
        this->dims = other.dims;
        std::copy(std::begin(other.data_),
                  std::end(other.data_),
                  std::begin(this->data_));
    }

    Matrix(Matrix&& other)
    {
        this->nelements_ = other.nelements_;
        this->nbytes_ = other.nbytes_;
        this->data_ = std::move(other.data_);
        this->dims = std::move(other.dims);
    }
};

// Specialization of the above for dynamically-allocated Matrix with Vector
// elements.
// XXX Only specialized for fixed-sized Vector elements at this time
template <typename Dtype, int32_t Length>
class Matrix<Vector<Dtype, Length>, Dynamic, Dynamic>
    : public MatrixBase<Vector<Dtype, Length>,
                        Dynamic,
                        Dynamic,
                        DynamicArrayWrapper<Vector<Dtype, Length>>>
{
public:
    using value_type = Vector<Dtype, Length>;

    // Need to use 'this' pointer below because templated base class members are
    // not visible in a certain phase of compilation. See here:
    // http://stackoverflow.com/a/6592617
    Matrix(int32_t rows, int32_t cols)
    {
        this->nelements_ = rows * cols;
        this->nbytes_ = this->nelements_ * Length * int32_t(sizeof(Dtype));
        this->data_ =
            DynamicArrayWrapper<Vector<Dtype, Length>>(this->nelements_);
        this->dims = {rows, cols};
    }

    Matrix(int32_t rows, int32_t cols, Vector<Dtype, Length> fill_value)
    {
        this->nelements_ = rows * cols;
        this->data_ =
            DynamicArrayWrapper<Vector<Dtype, Length>>(this->nelements_);
        this->nbytes_ = this->nelements_ * Length * int32_t(sizeof(Dtype));
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
        this->dims = {rows, cols};
    }

    Matrix(const Matrix& other)
    {
        this->nelements_ = other.nelements_;
        this->nbytes_ = other.nbytes_;
        this->data_ =
            DynamicArrayWrapper<Vector<Dtype, Length>>(this->nelements_);
        this->dims = other.dims;
        std::copy(std::begin(other.data_),
                  std::end(other.data_),
                  std::begin(this->data_));
    }

    Matrix(Matrix&& other)
    {
        this->nelements_ = other.nelements_;
        this->nbytes_ = other.nbytes_;
        this->data_ = std::move(other.data_);
        this->dims = std::move(other.dims);
    }
};

// Dynamic aliases
template <typename T>
using MatrixX = Matrix<T, Dynamic, Dynamic>;
using MatrixXd = MatrixX<double>;
using MatrixXi = MatrixX<int32_t>;
using MatrixXb = MatrixX<uint8_t>;

// Static aliases
template <typename T>
using Matrix44 = Matrix<T, 4, 4>;
using Matrix44d = Matrix44<double>;
template <typename T>
using Matrix33 = Matrix<T, 3, 3>;
using Matrix33i = Matrix33<int32_t>;
using Matrix33d = Matrix33<double>;
using Matrix33f = Matrix33<float>;
using Matrix33b = Matrix33<uint8_t>;
}

#endif
