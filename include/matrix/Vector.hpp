#pragma once

#ifndef SIPL_MATRIX_VECTOR_H
#define SIPL_MATRIX_VECTOR_H

#include "matrix/MatrixBase.hpp"

namespace sipl
{

// Partial specialization for static Vector type
template <typename Dtype, int32_t Length>
class Matrix<Dtype, Length, 1>
{
public:
    Matrix() : dims_({0, 0}), nelements_(0), data_() {}

    Matrix(std::initializer_list<Dtype> list)
        : dims_({Length, 1})
        , nelements_(Length)
        , nbytes_(sizeof(Dtype) * nelements_)
    {
        // std::initializer_list::size() is not marked constexpr, so can't use
        // this
        // static_assert(list.size() == Length, "initializer list size
        // mismatch");
        assert(list.size() == Length && "initializer_list size mismatch");
        std::copy(std::begin(list), std::end(list), std::begin(data_));
    }

    // Const accessor
    const Dtype& operator()(const size_t i) const { return data_[i]; }

    // Non-const accessor
    Dtype& operator()(const size_t i) { return data_[i]; }

    // Access elements by single index
    const Dtype& operator[](size_t index) const { return data_[index]; }

    Dtype& operator[](size_t index) { return data_[index]; }

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

    size_t size(void) const { return nelements_; }

    size_t size_in_bytes(void) const { return nbytes_; }

private:
    std::array<Dtype, 2> dims_;
    size_t nelements_;
    size_t nbytes_;
    std::array<Dtype, Length> data_;
};

// Statically allocated Vector
template <typename Dtype, int32_t Length>
using Vector = Matrix<Dtype, Length, 1>;

// Partial specialization for dynamic Vector type
template <typename Dtype>
class Matrix<Dtype, Dynamic, 1>
{
public:
    Matrix() : dims_({0, 0}), nelements_(0), data_(nullptr) {}

    Matrix(const size_t length)
        : dims_({length, 1})
        , nelements_(length)
        , nbytes_(sizeof(Dtype) * nelements_)
        , data_(std::unique_ptr<Dtype[]>(new Dtype[nelements_]))
    {
    }

    // Const accessor
    const Dtype& operator()(const size_t i) const { return data_[i]; }

    // Non-const accessor
    Dtype& operator()(const size_t i) { return data_[i]; }

    // Access elements by single index
    const Dtype& operator[](size_t index) const { return data_[index]; }

    Dtype& operator[](size_t index) { return data_[index]; }

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

    size_t size(void) const { return nelements_; }

    size_t size_in_bytes(void) const { return nbytes_; }

private:
    std::array<Dtype, 2> dims_;
    size_t nelements_;
    size_t nbytes_;
    std::unique_ptr<Dtype[]> data_;
};

// Aliases
// Dynamically-allocated vector
template <typename Dtype>
using VectorX = Vector<Dtype, Dynamic>;

template <typename Dtype>
using Vector3 = Vector<Dtype, 3>;
using Vector3b = Vector3<uint8_t>;
using RgbPixel = Vector3b;

template <typename T, int32_t N>
std::ostream& operator<<(std::ostream& s, const Vector<T, N>& v)
{
    s << "[";
    for (size_t i = 0; i < v.size() - 1; ++i) {
        s << std::to_string(v[i]) << ", ";
    }
    return s << std::to_string(v[v.size() - 1]) << "]";
}
}

#endif
