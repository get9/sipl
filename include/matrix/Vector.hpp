#pragma once

#ifndef SIPL_MATRIX_VECTOR_H
#define SIPL_MATRIX_VECTOR_H

#include <iostream>
#include <string>
#include <cassert>
#include "matrix/MatrixBase.hpp"

namespace sipl
{

// Partial specialization for static Vector type
template <typename Dtype, int32_t Length>
class Matrix<Dtype, Length, 1>
{
public:
    std::array<int32_t, Length> dims;

    Matrix() : dims({Length, 1}), nelements_(Length), data_() {}

    Matrix(std::initializer_list<Dtype> list)
        : dims({Length, 1})
        , nelements_(Length)
        , nbytes_(int32_t(sizeof(Dtype)) * nelements_)
    {
        // std::initializer_list::size() is not marked constexpr, so can't use
        // this
        // static_assert(list.size() == Length, "initializer list size
        // mismatch");
        assert(list.size() == Length && "initializer_list size mismatch");
        std::copy(std::begin(list), std::end(list), std::begin(data_));
    }

    // Fill vector with single scalar (useful for 0, etc)
    Matrix(const Dtype scalar)
        : dims({Length, 1})
        , nelements_(Length)
        , nbytes_(int32_t(sizeof(Dtype)) * nelements_)
    {
        std::fill(std::begin(data_), std::end(data_), scalar);
    }

    // Move constructor
    Matrix(Matrix&& other)
        : dims(std::move(other.dims))
        , nelements_(other.dims[0] * other.dims[1])
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::move(other.data_))
    {

        other.nelements_ = 0;
        other.nbytes_ = 0;
    }

    // Copy constructor
    Matrix(const Matrix& other)
        : dims(other.dims), nelements_(other.nelements_), nbytes_(other.nbytes_)
    {
        for (int32_t i = 0; i < other.size(); ++i) {
            data_[i] = other[i];
        }
    }

    // Const accessor
    const Dtype& operator()(const int32_t i) const { return data_[i]; }

    // Non-const accessor
    Dtype& operator()(const int32_t i) { return data_[i]; }

    // Access elements by single index
    const Dtype& operator[](int32_t index) const
    {
        // assert(index >= 0 && index < nelements_ && "out of range");
        return data_[index];
    }

    Dtype& operator[](int32_t index)
    {
        // assert(index >= 0 && index < nelements_ && "out of range");
        return data_[index];
    }

    // Raw accessor for data buffer
    const Dtype* buffer(void) const
    {
        return reinterpret_cast<const Dtype*>(data_.data());
    }

    Dtype* buffer(void) { return reinterpret_cast<Dtype*>(data_.data()); }

    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.data());
    }

    char* bytes(void) { return reinterpret_cast<char*>(data_.data()); }

    int32_t size(void) const { return nelements_; }

    int32_t size_in_bytes(void) const { return nbytes_; }

    Dtype max(void) const
    {
        return *std::max_element(std::begin(data_), std::end(data_));
    }

    Dtype min(void) const
    {
        return *std::min_element(std::begin(data_), std::end(data_));
    }

    template <typename T>
    Matrix<Dtype, Length, 1>& operator/=(const T scalar)
    {
        assert(scalar != 0 && "divide by zero error");
        for (auto&& d : data_) {
            d /= scalar;
        }
        return *this;
    }

    template <typename T>
    Matrix<Dtype, Length, 1>& operator=(Matrix<T, Length, 1> v)
    {
        for (int32_t i = 0; i < Length; ++i) {
            data_[i] = Dtype(v[i]);
        }
        return *this;
    }

private:
    int32_t nelements_;
    int32_t nbytes_;
    std::array<Dtype, Length> data_;
};

// Statically allocated Vector
template <typename Dtype, int32_t Length>
using Vector = Matrix<Dtype, Length, 1>;

template <typename T, int32_t Length>
Vector<double, Length> operator/(const Vector<T, Length>& v, const double s)
{
    Vector<double, Length> new_v;
    for (int32_t i = 0; i < Length; ++i) {
        new_v[i] = v[i] / s;
    }
    return new_v;
}

template <typename Scalar, typename T, int32_t Length>
Vector<Scalar, Length> operator*(const Scalar s, const Vector<T, Length>& v)
{
    Vector<Scalar, Length> new_v;
    for (int32_t i = 0; i < Length; ++i) {
        new_v[i] = s * v[i];
    }
    return new_v;
}

template <typename T, typename U, int32_t Length>
Vector<T, Length> operator+(const Vector<T, Length>& v1,
                            const Vector<U, Length>& v2)
{
    Vector<T, Length> new_v;
    for (int32_t i = 0; i < Length; ++i) {
        new_v[i] = v1[i] + v2[i];
    }
    return new_v;
}

// Other aliases of statically-allocated vectors
template <typename Dtype>
using Vector3 = Vector<Dtype, 3>;
using Vector3b = Vector3<uint8_t>;
using Vector3d = Vector3<double>;
using Vector3i = Vector3<int32_t>;
using RgbPixel = Vector3b;

// Partial specialization for dynamic Vector type
template <typename Dtype>
class Matrix<Dtype, Dynamic, 1>
{
public:
    const std::array<int32_t, 2> dims;

    Matrix() : dims({0, 0}), nelements_(0), data_(nullptr) {}

    Matrix(const int32_t length)
        : dims({length, 1})
        , nelements_(int32_t(length))
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::unique_ptr<Dtype[]>(new Dtype[nelements_]()))
    {
    }

    // Move constructor
    Matrix(Matrix&& other)
        : dims(std::move(other.dims))
        , nelements_(dims[0] * dims[1])
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::move(other.data_))
    {
        other.nelements_ = 0;
        other.nbytes_ = 0;
        other.data_ = nullptr;
    }

    // Copy constructor
    Matrix(const Matrix& other)
        : dims(other.dims), nelements_(other.nelements_), nbytes_(other.nbytes_)
    {
        for (int32_t i = 0; i < other.size(); ++i) {
            data_[i] = other[i];
        }
    }

    // Const accessor
    const Dtype& operator()(const int32_t i) const { return data_[i]; }

    // Non-const accessor
    Dtype& operator()(const int32_t i) { return data_[i]; }

    // Access elements by single index
    const Dtype& operator[](int32_t index) const { return data_[index]; }

    Dtype& operator[](int32_t index) { return data_[index]; }

    // Raw accessor for data buffer
    const Dtype* buffer(void) const
    {
        return reinterpret_cast<const Dtype*>(data_.data());
    }

    Dtype* buffer(void) { return reinterpret_cast<Dtype*>(data_.data()); }

    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.data());
    }

    char* bytes(void) { return reinterpret_cast<char*>(data_.data()); }

    int32_t size(void) const { return nelements_; }

    int32_t size_in_bytes(void) const { return nbytes_; }

    Dtype max(void) const
    {
        Dtype max = data_[0];
        for (int32_t i = 1; i < nelements_; ++i) {
            if (data_[i] > max) {
                max = data_[i];
            }
        }
        return max;
    }

    int32_t max_element(void) const
    {
        int32_t max_idx = 0;
        for (int32_t i = 1; i < nelements_; ++i) {
            if (data_[i] > data_[max_idx]) {
                max_idx = i;
            }
        }
        return max_idx;
    }

    Dtype min(void) const
    {
        Dtype min = std::numeric_limits<Dtype>::max();
        for (int32_t i = 0; i < nelements_; ++i) {
            if (data_[i] < min) {
                min = data_[i];
            }
        }
        return max;
    }

    int32_t min_element(void) const
    {
        int32_t min_idx = 0;
        for (int32_t i = 1; i < nelements_; ++i) {
            if (data_[i] < data_[min_idx]) {
                min_idx = i;
            }
        }
        return min_idx;
    }

private:
    int32_t nelements_;
    int32_t nbytes_;
    std::unique_ptr<Dtype[]> data_;
};

// Aliases
// Dynamically-allocated vector
template <typename Dtype>
using VectorX = Vector<Dtype, Dynamic>;
using VectorXd = VectorX<double>;

template <typename T>
VectorX<double> operator/(const VectorX<T>& v, const double s)
{
    VectorXd new_v(v.size());
    for (int32_t i = 0; i < v.size(); ++i) {
        new_v[i] = v[i] / s;
    }
    return new_v;
}

template <typename T, int32_t N>
std::ostream& operator<<(std::ostream& s, const Vector<T, N>& v)
{
    s << "[";
    for (int32_t i = 0; i < v.size() - 1; ++i) {
        s << std::to_string(v[i]) << ", ";
    }
    return s << std::to_string(v[v.size() - 1]) << "]";
}
}

#endif
