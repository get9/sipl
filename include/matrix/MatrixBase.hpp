#pragma once

#ifndef SIPL_MATRIX_MATRIXBASE_H
#define SIPL_MATRIX_MATRIXBASE_H

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <array>
#include <sstream>
#include "Constants.hpp"
#include "matrix/VectorOps.hpp"

namespace sipl
{

// Statically-sized matrices
template <typename Dtype, int32_t Rows, int32_t Cols, typename Container>
class MatrixBase
{
public:
    std::array<int32_t, 2> dims;

    // Constructors all come from derived classes

    // Iterator & element access
    Dtype* begin() { return std::begin(data_); }
    const Dtype* begin() const { return std::begin(data_); }

    Dtype* end() { return std::end(data_); }
    const Dtype* end() const { return std::end(data_); }

    Dtype& front() { return data_.front(); }
    const Dtype& front() const { return data_.front(); }

    Dtype& back() { return data_.back(); }
    const Dtype& back() const { return data_.back(); }

    const Dtype& operator()(const int32_t row, const int32_t col) const
    {
        assert(row >= 0 && row < dims[0] && "out of range");
        assert(col >= 0 && col < dims[1] && "out of range");
        return data_[row * dims[1] + col];
    }
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
    const Dtype* data(void) const
    {
        return reinterpret_cast<const Dtype*>(data_.data());
    }
    Dtype* data(void) { return reinterpret_cast<Dtype*>(data_.data()); }

    // Serialization to byte array
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.data());
    }
    char* as_bytes(void) { return reinterpret_cast<char*>(data_.data()); }

    int32_t size(void) const { return nelements_; }

    int32_t size_in_bytes(void) const { return nbytes_; }

    template <typename Functor>
    void apply(Functor f)
    {
        std::transform(
            std::begin(data_), std::end(data_), std::begin(data_), f);
    }

    // Apply a functor to each elment returning a new modified vector
    // XXX Revisit this to figure out if there's a way to name them both the
    // same thing without the compiler resolving the overload to the non-const
    // version
    template <typename Functor>
    MatrixBase apply_clone(Functor f) const
    {
        MatrixBase new_v(dims[0], dims[1]);
        std::transform(
            std::begin(data_), std::end(data_), std::begin(new_v), f);
        return new_v;
    }

    // Cast operator
    /*
    template <typename CastType>
    MatrixBase<CastType, Rows, Cols> as_type() const
    {
        MatrixBase<CastType, Rows, Cols> new_mat;
        for (int32_t i = 0; i < nelements_; ++i) {
            new_mat[i] = CastType(data_[i]);
        }
        return new_mat;
    }
    */

    // Scalar math manipulation
    template <typename Scalar>
    MatrixBase& operator/=(Scalar s)
    {
        apply([s](Dtype e) { return e / s; });
        return *this;
    }

    template <typename Scalar>
    MatrixBase& operator*=(Scalar s)
    {
        apply([s](Dtype e) { return e * s; });
        return *this;
    }

    template <typename Scalar>
    MatrixBase& operator+=(Scalar s)
    {
        apply([s](Dtype e) { return e + s; });
        return *this;
    }

    template <typename Scalar>
    MatrixBase& operator-=(Scalar s)
    {
        apply([s](Dtype e) { return e - s; });
        return *this;
    }

    std::string str() const
    {
        std::stringstream ss;
        ss << "[";
        for (int32_t i = 0; i < dims[0] - 1; ++i) {
            for (int32_t j = 0; j < dims[1] - 1; ++j) {
                ss << (*this)(i, j) << " ";
            }
            ss << (*this)(i, dims[1] - 1) << ";" << std::endl;
        }
        for (int32_t j = 0; j < dims[1] - 1; ++j) {
            ss << (*this)(dims[0] - 1, j) << " ";
        }
        ss << (*this)(dims[0] - 1, dims[1] - 1) << "]";
        return ss.str();
    }

protected:
    int32_t nelements_;
    int32_t nbytes_;
    Container data_;
};
}

#endif
