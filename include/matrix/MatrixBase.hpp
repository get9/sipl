#pragma once

#ifndef SIPL_MATRIX_MATRIXBASE_H
#define SIPL_MATRIX_MATRIXBASE_H

#include <array>
#include <initializer_list>
#include <limits>
#include <cmath>
#include "Constants.hpp"

namespace sipl
{
// Statically-sized matrices
template <typename Dtype, int32_t Rows, int32_t Cols>
class Matrix
{
public:
    const std::array<size_t, 2> dims;
    const size_t rows;
    const size_t cols;

    // Default constructor
    Matrix()
        : dims({Rows, Cols})
        , rows(Rows)
        , cols(Cols)
        , nelements_(Rows * Cols)
        , nbytes_(nelements_ * sizeof(Dtype))
        , data_({0})
    {
    }

    // Initializer list constructor
    Matrix(std::initializer_list<std::initializer_list<Dtype>> list)
        : dims({Rows, Cols})
        , rows(Rows)
        , cols(Cols)
        , nelements_(Rows * Cols)
        , nbytes_(nelements_ * sizeof(Dtype))
        , data_({0})

    {
        // Safety check
        static_assert(list.size() == Rows, "initializer list size mismatch");
        for (const auto l : list) {
            static_assert(l.size() == Cols, "initializer list size mismatch");
        }

        // Fill data from initializer_lists
        for (size_t i = 0; i < Rows; ++i) {
            for (size_t j = 0; j < Cols; ++i) {
                data_[i * Cols + j] = list[i][j];
            }
        }
    }

    // Const accessor
    const Dtype& operator()(const size_t row, const size_t col) const
    {
        return data_[row * cols + col];
    }

    // Non-const accessor
    Dtype& operator()(const size_t row, const size_t col)
    {
        return data_[row * cols + col];
    }

    // Access elements by single index
    const Dtype& operator[](size_t index) const { return data_[index]; }

    Dtype& operator[](size_t index) { return data_[index]; }

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

    char* bytes(void) { return reinterpret_cast<char*>(data_.data()); }

    size_t size(void) const { return nelements_; }

    size_t size_in_bytes(void) const { return nbytes_; }

    /*
    Vector<Dtype> operator*(const Vector<Dtype>& v) const
    {
        assert(cols == v.size() && "cols and vector size do not match up");

        // For clamping
        constexpr auto max = std::numeric_limits<Dtype>::max();
        constexpr auto min = std::numeric_limits<Dtype>::min();

        Vector<Dtype> new_v(v.size());
        for (size_t row = 0; row < rows; ++row) {
            for (size_t col = 0; col < cols; ++col) {
                double sum = 0;
                if (std::round(sum) >= max)) {
                        sum = max;
                    }
                else if (std::round(sum) = < min) {
                    sum = min;
                } else {
                    sum += (*this)(row, col) * v[col];
                }
            }
            new_v[row] = Dtype(std::round(sum));
        }
    }
    */

private:
    size_t nelements_;
    size_t nbytes_;
    std::unique_ptr<Dtype[]> data_;
};

// Matrix mul for statically-defined types
template <typename Dtype, int32_t R1, int32_t C1, int32_t R2, int32_t C2>
Matrix<Dtype, R1, C2> operator*(const Matrix<Dtype, R1, C1>& m1,
                                const Matrix<Dtype, R2, C2>& m2)
{
    static_assert(
        !(R1 == Dynamic || C1 == Dynamic || R2 == Dynamic || C2 == Dynamic),
        "wrong matrix mul routine used - this is for static matrices");
    static_assert(C1 == R2, "m1 cols must equal m2 rows");

    // For clamping
    constexpr auto max = std::numeric_limits<Dtype>::max();
    constexpr auto min = std::numeric_limits<Dtype>::min();

    Matrix<Dtype, R1, C2> mat;
    for (size_t row = 0; row < m1.rows; ++row) {
        for (size_t col = 0; col < m2.cols; ++col) {
            double sum = 0;
            for (size_t inner = 0; inner < m1.cols; ++inner) {
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
}

#endif
