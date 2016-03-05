#pragma once

#ifndef SIPL_MATRIX_MATRIX_H
#define SIPL_MATRIX_MATRIX_H

#include "matrix/Wrappers.hpp"
#include "matrix/Vector.hpp"
#include "matrix/MatrixBase.hpp"

namespace sipl
{

// Need to use 'this' pointer below because templated base class members are
// not visible in a certain phase of compilation. See here:
// http://stackoverflow.com/a/6592617

// Initial class for statically-allocated matrices
template <typename Dtype, int32_t Rows, int32_t Cols>
class Matrix : public MatrixBase<Dtype,
                                 Rows,
                                 Cols,
                                 StaticArrayWrapper<Dtype, Rows * Cols>>
{
public:
    using ContainerType = StaticArrayWrapper<Dtype, Rows * Cols>;
    using BaseClass = MatrixBase<Dtype, Rows, Cols, ContainerType>;
    using BaseClass::BaseClass;

    Matrix()
    {
        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType();
        this->dims = {Rows, Cols};
    }

    Matrix(Dtype fill_value)
    {
        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType();
        this->dims = {Rows, Cols};
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
    {
        assert(int32_t(list.size()) == Rows && "size mismatch");
        for (const auto l : list) {
            assert(int32_t(l.size()) == Cols && "size mismatch");
        }

        this->nelements_ = Rows * Cols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType();
        this->dims = {Rows, Cols};
        int32_t i = 0;
        for (const auto l : list) {
            std::copy(std::begin(l), std::end(l),
                      std::begin(this->data_) + i++ * l.size());
        }
    }

    // Convert one matrix type to another
    template <typename OtherType>
    Matrix(const Matrix<OtherType, Rows, Cols>& other)
    {
        this->dims = other.dims;
        this->nelements_ = other.size();
        this->nbytes_ = other.size_in_bytes();
        this->data_ = ContainerType();
        std::transform(std::begin(other), std::end(other),
                       std::begin(this->data_),
                       [](auto e) { return Dtype(e); });
    }
};

// Specialization of the above for dynamically-allocated matrices.
template <typename Dtype>
class Matrix<Dtype, Dynamic, Dynamic>
    : public MatrixBase<Dtype,
                        Dynamic,
                        Dynamic,
                        DynamicArrayWrapper<Dtype, Dynamic>>
{
public:
    using ContainerType = DynamicArrayWrapper<Dtype, Dynamic>;
    using BaseClass = MatrixBase<Dtype, Dynamic, Dynamic, ContainerType>;
    using BaseClass::BaseClass;

    Matrix(int32_t rows, int32_t cols)
    {
        this->nelements_ = rows * cols;
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->dims = {rows, cols};
    }

    Matrix(int32_t rows, int32_t cols, Dtype fill_value)
    {
        this->nelements_ = rows * cols;
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->dims = {rows, cols};
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    Matrix(std::array<int32_t, 2> new_dims)
    {
        this->nelements_ = new_dims[0] * new_dims[1];
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->dims = new_dims;
    }

    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
    {
        int32_t nrows = list.size();
        int32_t ncols = list.begin()->size();
        for (const auto l : list) {
            assert(int32_t(l.size()) == ncols &&
                   "initializer_list size mismatch");
        }

        this->nelements_ = nrows * ncols;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType(this->nelements_);
        this->dims = {nrows, ncols};
        int32_t i = 0;
        for (const auto l : list) {
            std::copy(std::begin(l), std::end(l),
                      std::begin(this->data_) + i++ * l.size());
        }
    }

    // Convert one matrix type to another
    template <typename OtherType>
    Matrix(const Matrix<OtherType, Dynamic, Dynamic>& other)
    {
        this->dims = other.dims;
        this->nelements_ = other.size();
        this->nbytes_ = other.size_in_bytes();
        this->data_ = ContainerType(this->nelements_);
        std::transform(std::begin(other), std::end(other),
                       std::begin(this->data_),
                       [](auto e) { return Dtype(e); });
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
                        DynamicArrayWrapper<Vector<Dtype, Length>, Length>>
{
public:
    using value_type = Vector<Dtype, Length>;
    using ContainerType = DynamicArrayWrapper<value_type, Length>;
    // XXX Might not let me do this...
    using BaseClass = MatrixBase<value_type, Dynamic, Dynamic, ContainerType>;
    using BaseClass::BaseClass;

    Matrix(int32_t rows, int32_t cols)
    {
        this->nelements_ = rows * cols;
        this->nbytes_ = this->nelements_ * Length * int32_t(sizeof(Dtype));
        this->data_ = ContainerType(this->nelements_);
        this->dims = {rows, cols};
    }

    Matrix(int32_t rows, int32_t cols, value_type fill_value)
    {
        this->nelements_ = rows * cols;
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * Length * int32_t(sizeof(Dtype));
        this->dims = {rows, cols};
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    // Convert one matrix type to another
    template <typename OtherType>
    Matrix(const Matrix<Vector<OtherType, Length>, Dynamic, Dynamic>& other)
    {
        this->dims = other.dims;
        this->nelements_ = other.size();
        this->nbytes_ = other.size_in_bytes();
        this->data_ = ContainerType(this->nelements_);
        std::transform(std::begin(other), std::end(other),
                       std::begin(this->data_),
                       [](auto e) { return value_type(e); });
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
