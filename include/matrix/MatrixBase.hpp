#pragma once

#ifndef SIPL_MATRIX_MATRIXBASE_H
#define SIPL_MATRIX_MATRIXBASE_H

#include <array>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include "Constants.hpp"
#include "matrix/Common.hpp"

namespace sipl
{

// Statically-sized matrices
template <typename Dtype, int32_t Rows, int32_t Cols, typename Container>
class MatrixBase
{
public:
    using value_type = Dtype;

    std::array<int32_t, 2> dims;

    MatrixBase()
        : dims({Rows, Cols})
        , nelements_(Rows * Cols)
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(Rows * Cols)
    {
    }

    MatrixBase(const MatrixBase& other)
        : dims(other.dims)
        , nelements_(other.nelements_)
        , nbytes_(other.nbytes_)
        , data_(other.data_)
    {
    }

    MatrixBase(MatrixBase&& other)
        : dims(std::move(other.dims))
        , nelements_(other.nelements_)
        , nbytes_(other.nbytes_)
        , data_(std::move(other.data_))
    {
    }

    // Convert one matrix type to another
    template <typename OtherType>
    MatrixBase(const MatrixBase<OtherType, Rows, Cols, Container>& other)
        : dims(other.dims)
        , nelements_(other.nelements_)
        , nbytes_(other.nbytes_)
        , data_(other.nelements_)
    {
        std::transform(std::begin(other), std::end(other), std::begin(data_),
                       [](auto e) { return Dtype(e); });
    }

    // Iterator & element access
    Dtype* begin() { return std::begin(data_); }
    const Dtype* begin() const { return std::begin(data_); }

    Dtype* end() { return std::end(data_); }
    const Dtype* end() const { return std::end(data_); }

    Dtype& front() { return data_.front(); }
    const Dtype& front() const { return data_.front(); }

    Dtype& back() { return data_.back(); }
    const Dtype& back() const { return data_.back(); }

    const Dtype& operator()(int32_t row, int32_t col) const
    {
        assert(row >= 0 && row < dims[0] && "out of range");
        assert(col >= 0 && col < dims[1] && "out of range");
        return data_[row * dims[1] + col];
    }
    Dtype& operator()(int32_t row, int32_t col)
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

    template <typename UnaryFunctor>
    void transform(UnaryFunctor f)
    {
        std::transform(std::begin(data_), std::end(data_), std::begin(data_),
                       f);
    }

    // Scalar math manipulation
    template <typename Scalar>
    MatrixBase<Dtype, Rows, Cols, Container>& operator/=(Scalar s)
    {
        transform([s](Dtype e) { return e / s; });
        return *this;
    }

    template <typename Scalar>
    MatrixBase<Dtype, Rows, Cols, Container>& operator*=(Scalar s)
    {
        transform([s](Dtype e) { return e * s; });
        return *this;
    }

    template <typename Scalar>
    MatrixBase<Dtype, Rows, Cols, Container>& operator+=(Scalar s)
    {
        transform([s](Dtype e) { return e + s; });
        return *this;
    }

    template <typename Scalar>
    MatrixBase<Dtype, Rows, Cols, Container>& operator-=(Scalar s)
    {
        transform([s](Dtype e) { return e - s; });
        return *this;
    }

    std::string str() const
    {
        std::stringstream ss;
        ss << "[";
        for (int32_t i = 0; i < dims[0] - 1; ++i) {
            for (int32_t j = 0; j < dims[1] - 1; ++j) {
                ss << std::to_string((*this)(i, j)) << " ";
            }
            ss << std::to_string((*this)(i, dims[1] - 1)) << ";" << std::endl;
        }
        for (int32_t j = 0; j < dims[1] - 1; ++j) {
            ss << std::to_string((*this)(dims[0] - 1, j)) << " ";
        }
        ss << std::to_string((*this)(dims[0] - 1, dims[1] - 1)) << "]";
        return ss.str();
    }

    Dtype sum() const
    {
        // Sort so we lessen floating point error
        MatrixBase tmp(*this);
        std::sort(std::begin(tmp), std::end(tmp));
        return std::accumulate(std::begin(tmp), std::end(tmp), Dtype(0),
                               [](Dtype sum, Dtype e) { return sum + e; });
    }

    Dtype abssum() const
    {
        // Sort so we lessen floating point error
        MatrixBase tmp(*this);
        std::sort(std::begin(tmp), std::end(tmp));
        return std::accumulate(
            std::begin(tmp), std::end(tmp), Dtype(0),
            [](Dtype sum, Dtype e) { return sum + std::abs(e); });
    }

    // Conversion operator
    template <typename T>
    operator MatrixBase<T, Rows, Cols, Container>() const
    {
        MatrixBase<T, Rows, Cols, Container> new_m(dims);
        new_m.transform([](auto e) { return T(e); });
        return new_m;
    }

    friend std::ostream& operator<<(std::ostream& s, const MatrixBase& m)
    {
        return s << m.str();
    }

    // Min/max operations
    Dtype max(void) const
    {
        auto ret = std::max_element(std::begin(data_), std::end(data_));
        if (ret == std::end(data_)) {
            throw std::range_error("empty vector");
        }
        return *ret;
    }

    Dtype min(void) const
    {
        auto ret = std::min_element(std::begin(data_), std::end(data_));
        if (ret == std::end(data_)) {
            throw std::range_error("empty vector");
        }
        return *ret;
    }

    int32_t argmax() const
    {
        auto ret = std::max_element(std::begin(data_), std::end(data_));
        if (ret == std::end(data_)) {
            throw std::range_error("empty vector");
        }
        return ret - std::begin(data_);
    }

    int32_t argmin() const
    {
        auto ret = std::min_element(std::begin(data_), std::end(data_));
        if (ret == std::end(data_)) {
            throw std::range_error("empty vector");
        }
        return ret - std::begin(data_);
    }

protected:
    int32_t nelements_;
    int32_t nbytes_;
    Container data_;

    // Helper functions to clamp a row/col index to in-bounds
    int32_t clamp_row_index(int32_t index) const
    {
        if (index < 0) {
            return 0;
        } else if (index >= dims[0]) {
            return dims[0] - 1;
        } else {
            return index;
        }
    }
    int32_t clamp_col_index(int32_t index) const
    {
        if (index < 0) {
            return 0;
        } else if (index >= dims[1]) {
            return dims[1] - 1;
        } else {
            return index;
        }
    }
};
}

#endif
