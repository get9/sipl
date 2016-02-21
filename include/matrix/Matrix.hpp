#pragma once

#ifndef SIPL_MATRIX_H
#define SIPL_MATRIX_H

#include <memory>
#include "matrix/MatrixBase.hpp"
#include "matrix/Vector.hpp"

namespace sipl
{

// Dynaically-sized matrices
template <typename Dtype>
class Matrix<Dtype, Dynamic, Dynamic>
{
public:
    const std::array<int32_t, 2> dims;
    const int32_t rows;
    const int32_t cols;

    // Default constructor, no data
    Matrix(const int32_t rs, const int32_t cs)
        : dims({rs, cs})
        , rows(rs)
        , cols(cs)
        , nelements_(int32_t(rs * cs))
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::unique_ptr<Dtype[]>(new Dtype[nelements_]))
    {
    }

    // Const accessor
    const Dtype& operator()(const int32_t row, const int32_t col) const
    {
        return data_[row * cols + col];
    }

    // Non-const accessor
    Dtype& operator()(const int32_t row, const int32_t col)
    {
        return data_[row * cols + col];
    }

    // Access elements by single index
    const Dtype& operator[](int32_t index) const { return data_[index]; }

    Dtype& operator[](int32_t index) { return data_[index]; }

    // Raw accessor for data buffer
    const Dtype* buffer(void) const
    {
        return reinterpret_cast<const Dtype*>(data_.get());
    }

    Dtype* buffer(void) { return reinterpret_cast<Dtype*>(data_.get()); }

    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.get());
    }

    char* bytes(void) { return reinterpret_cast<char*>(data_.get()); }

    int32_t size(void) const { return nelements_; }

    int32_t size_in_bytes(void) const { return nbytes_; }

private:
    int32_t nelements_;
    int32_t nbytes_;
    std::unique_ptr<Dtype[]> data_;
};

// Dynamically-allocated matrix
template <typename Dtype>
using MatrixX = Matrix<Dtype, Dynamic, Dynamic>;

// Matrix mul for dynamically-defined matrices
template <typename Dtype, int32_t R1, int32_t C2>
Matrix<Dtype, R1, C2> operator*(const Matrix<Dtype, R1, Dynamic>& m1,
                                const Matrix<Dtype, Dynamic, C2>& m2)
{
    assert(m1.cols == m2.rows && "m1 cols must equal m2 rows");

    // For clamping
    constexpr auto max = std::numeric_limits<Dtype>::max();
    constexpr auto min = std::numeric_limits<Dtype>::min();

    Matrix<Dtype, Dynamic, Dynamic> mat(m1.rows, m2.cols);
    for (int32_t row = 0; row < m1.rows; ++row) {
        for (int32_t col = 0; col < m2.cols; ++col) {
            double sum = 0;
            for (int32_t inner = 0; inner < m1.cols; ++inner) {
                if (std::round(sum) >= max) {
                    sum = max;
                } else if (std::round(sum) <= min) {
                    sum = min;
                } else {
                    sum += m1(row, inner) * m2(inner, col);
                }
            }
            mat(row, col) = Dtype(std::round(sum));
        }
    }
    return mat;
}

template <typename T, typename Scalar>
Matrix<Scalar, Dynamic, Dynamic> operator/(const Matrix<T, Dynamic, Dynamic> m,
                                           const Scalar s)
{
    assert((s - 1e-10) != 0 && "precision issues");
    Matrix<Scalar, Dynamic, Dynamic> new_m(m.dims[0], m.dims[1]);
    for (int32_t i = 0; i < m.dims[0]; ++i) {
        for (int32_t j = 0; j < m.dims[1]; ++j) {
            new_m(i, j) = Scalar(m(i, j) / s);
        }
    }

    return new_m;
}

template <typename T, typename Scalar>
Matrix<Scalar, Dynamic, Dynamic> operator*(const Scalar s,
                                           const Matrix<T, Dynamic, Dynamic> m)
{
    assert((s - 1e-10) != 0 && "precision issues");
    Matrix<Scalar, Dynamic, Dynamic> new_m(m.rows, m.cols);
    for (int32_t i = 0; i < m.dims[0]; ++i) {
        for (int32_t j = 0; j < m.dims[1]; ++j) {
            new_m(i, j) = Scalar(s * m(i, j));
        }
    }

    return new_m;
}

template <typename T, typename Scalar>
Matrix<Scalar, Dynamic, Dynamic> operator*(const Matrix<T, Dynamic, Dynamic> m,
                                           const Scalar s)

{
    assert((s - 1e-10) != 0 && "precision issues");
    return s * m;
}
}

#endif
