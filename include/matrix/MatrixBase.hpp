#pragma once

#ifndef SIPL_MATRIX_MATRIXBASE_H
#define SIPL_MATRIX_MATRIXBASE_H

#include <array>
#include <initializer_list>
#include <limits>
#include <cmath>
#include <iostream>
#include <cassert>
#include "Constants.hpp"
#include "matrix/Vector.hpp"

namespace sipl
{
// Statically-sized matrices
template <typename Dtype, int32_t Rows, int32_t Cols>
class Matrix
{
public:
    const std::array<int32_t, 2> dims = {Rows, Cols};

    // Default constructor
    Matrix()
        : nelements_(Rows * Cols)
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_({0})
    {
    }

    // Initializer list constructor
    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
        : nelements_(Rows * Cols), nbytes_(nelements_ * int32_t(sizeof(Dtype)))

    {
        // Safety check
        // XXX Since std::initializer_list::size() is not marked constexpr, we
        // must use regular assert() instead of static_assert()
        assert(list.size() == Rows && "initializer list size mismatch");
        size_t i = 0;
        for (const auto& l : list) {
            assert(l.size() == Cols && "initializer list size mismatch");
            std::copy(
                std::begin(l), std::end(l), std::begin(data_) + i++ * l.size());
        }
    }

    Matrix(Matrix&& other)
        : nelements_(other.dims[0] * other.dims[1])
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::move(other.data_))
    {
    }

    Matrix(const Matrix& other)
    {
        for (int32_t i = 0; i < other.size(); ++i) {
            data_[i] = other[i];
        }
    }

    // Const accessor
    const Dtype& operator()(const int32_t row, const int32_t col) const
    {
        assert(row >= 0 && row < dims[0] && "out of range");
        assert(col >= 0 && col < dims[1] && "out of range");
        return data_[row * dims[1] + col];
    }

    // Non-const accessor
    Dtype& operator()(const int32_t row, const int32_t col)
    {
        assert(row >= 0 && row < dims[0] && "out of range");
        assert(col >= 0 && col < dims[1] && "out of range");
        return data_[row * dims[1] + col];
    }

    // Access elements by single index
    const Dtype& operator[](int32_t index) const
    {
        assert(index >= 0 && index < nelements_ && "out of range");
        return data_[index];
    }

    Dtype& operator[](int32_t index)
    {
        assert(index >= 0 && index < nelements_ && "out of range");
        return data_[index];
    }

    // Raw accessor for data buffer
    const Dtype* buffer(void) const
    {
        return reinterpret_cast<const Dtype*>(data_);
    }

    Dtype* buffer(void) { return reinterpret_cast<Dtype*>(data_.data()); }

    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_);
    }

    char* as_bytes(void) { return reinterpret_cast<char*>(data_.data()); }

    int32_t size(void) const { return nelements_; }

    int32_t size_in_bytes(void) const { return nbytes_; }

    // Cast operator
    template <typename CastType>
    Matrix<CastType, Rows, Cols> as_type() const
    {
        Matrix<CastType, Rows, Cols> new_mat;
        for (int32_t i = 0; i < nelements_; ++i) {
            new_mat[i] = CastType(data_[i]);
        }
        return new_mat;
    }

    // Operator = for static matrices
    template <typename T>
    Matrix<Dtype, Rows, Cols>& operator=(Matrix<T, Rows, Cols> other)
    {
        for (int32_t i = 0; i < nelements_; ++i) {
            data_[i] = other[i];
        }
        return *this;
    }

    template <typename Scalar>
    Matrix<Dtype, Rows, Cols>& operator/=(const Scalar s)
    {
        for (int32_t i = 0; i < dims[0]; ++i) {
            for (int32_t j = 0; j < dims[1]; ++j) {
                (*this)(i, j) /= s;
            }
        }
        return *this;
    }

    // Matrix mul for statically-defined types
    template <typename T, int32_t R2, int32_t C2>
    Matrix<Dtype, Rows, C2> operator*(const Matrix<T, R2, C2>& m2) const
    {
        static_assert(
            C2 != Dynamic,
            "wrong matrix mul routine used - this is for static matrices");
        static_assert(R2 == dims[1], "size mismatch");

        Matrix<Dtype, Rows, C2> mat;
        for (int32_t row = 0; row < dims[0]; ++row) {
            for (int32_t col = 0; col < m2.dims[1]; ++col) {
                Dtype sum = 0;
                for (int32_t inner = 0; inner < dims[1]; ++inner) {
                    sum += data_[row * dims[1] + inner] *
                           m2[inner * m2.dims[1] + col];
                }
                mat[col * m2.dims[1] + row] = sum;
            }
        }
        return mat;
    }

    // Matrix mul for statically-defined vectors
    template <typename T, int32_t Length>
    Matrix<Dtype, Rows, 1> operator*(const Matrix<T, Length, 1>& v) const
    {
        static_assert(Length == Cols,
                      "Vector must be the same length as number of columns");
        Matrix<Dtype, Rows, 1> res;
        for (int32_t row = 0; row < dims[0]; ++row) {
            Dtype sum = 0;
            for (int32_t col = 0; col < Length; ++col) {
                sum += data_[row * dims[1] + col] * v[col];
            }
            res[row] = sum;
        }

        return res;
    }

private:
    int32_t nelements_;
    int32_t nbytes_;
    std::array<Dtype, Rows * Cols> data_;
};

template <typename T, typename Scalar, int32_t Rows, int32_t Cols>
Matrix<Scalar, Rows, Cols> operator/(const Matrix<T, Rows, Cols> m,
                                     const Scalar s)
{
    assert(std::fabs(s - 1e-10) >= 0 && "precision issues");
    Matrix<Scalar, Rows, Cols> new_m;
    for (int32_t i = 0; i < m.dims[0]; ++i) {
        for (int32_t j = 0; j < m.dims[1]; ++j) {
            new_m(i, j) = Scalar(m(i, j) / s);
        }
    }

    return new_m;
}

template <typename T, typename Scalar, int32_t Rows, int32_t Cols>
Matrix<Scalar, Rows, Cols> operator*(const Scalar s,
                                     const Matrix<T, Rows, Cols> m)
{
    assert(std::fabs(s - 1e-10) >= 0 && "precision issues");
    Matrix<Scalar, Rows, Cols> new_m;
    for (int32_t i = 0; i < m.dims[0]; ++i) {
        for (int32_t j = 0; j < m.dims[1]; ++j) {
            new_m(i, j) = Scalar(s * m(i, j));
        }
    }

    return new_m;
}

template <typename T, typename Scalar, int32_t Rows, int32_t Cols>
Matrix<Scalar, Rows, Cols> operator*(const Matrix<T, Rows, Cols> m,
                                     const Scalar s)

{
    assert((s - 1e-10) != 0 && "precision issues");
    return s * m;
}

template <typename T, typename U, int32_t Rows, int32_t Cols>
Vector<U, Rows> operator*(const Matrix<T, Rows, Cols>& m,
                          const Vector<U, Cols>& v)
{
    Vector<U, Rows> new_v;
    for (int32_t i = 0; i < m.dims[0]; ++i) {
        U sum = 0;
        for (int32_t j = 0; j < m.dims[1]; ++j) {
            sum += m(i, j) * v(j);
        }
        new_v[i] = sum;
    }
    return new_v;
}

template <typename Dtype>
std::string as_string(const Dtype d)
{
    return std::to_string(d);
}

// Aliases for sized matrices
template <typename Dtype>
using Matrix33 = Matrix<Dtype, 3, 3>;
using Matrix33d = Matrix33<double>;
using Matrix33f = Matrix33<float>;
using Matrix33i = Matrix33<int32_t>;

template <typename T, int32_t Rows, int32_t Cols>
std::ostream& operator<<(std::ostream& s, const Matrix<T, Rows, Cols>& m)
{
    for (int32_t i = 0; i < m.dims[0] - 1; ++i) {
        for (int32_t j = 0; j < m.dims[1]; ++j) {
            s << as_string(m(i, j)) << " ";
        }
        s << std::endl;
    }
    for (int32_t j = 0; j < m.dims[1]; ++j) {
        s << as_string(m(m.dims[0] - 1, j)) << " ";
    }
    return s;
}
}

#endif
