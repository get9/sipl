#pragma once

#ifndef SIPL_MATRIX_VECTOR_H
#define SIPL_MATRIX_VECTOR_H

#include <iostream>
#include <algorithm>
#include <memory>
#include <array>
#include <string>
#include <cassert>
#include "matrix/VectorBase.hpp"
#include "Constants.hpp"

namespace sipl
{

// Wrapper for std::unique_ptr<Dtype[]>. Need this so we can use the regular
// VectorBase calls
template <typename Dtype>
struct UniquePtrWrapper {
    std::unique_ptr<Dtype[]> data_;
    int32_t size_;

    UniquePtrWrapper() : data_(nullptr) {}

    UniquePtrWrapper(int32_t size) : data_(new Dtype[size]), size_(size) {}

    UniquePtrWrapper(const UniquePtrWrapper& other) : size_(other.size)
    {
        std::copy(
            std::begin(other.data_), std::end(other.data_), std::begin(data_));
    }

    UniquePtrWrapper(UniquePtrWrapper&& other)
        : data_(std::move(other.data_)), size_(other.size_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    Dtype* begin() { return data_.get(); }

    const Dtype* begin() const { return data_.get(); }

    Dtype* end() { return data_.get() + size_; }

    const Dtype* end() const { return data_.get() + size_; }

    int32_t size() const { return size_; }

    Dtype& operator[](int32_t index) { return data_[index]; }

    const Dtype& operator[](int32_t index) const { return data_[index]; }

    Dtype* data() { return data_.get(); }

    const Dtype* data() const { return data_.get(); }
};

// Derived class for Vector. Note that it doesn't need to do anything because
// everything's already implemented by BaseVector
template <typename Dtype, int32_t Length>
class Vector : public VectorBase<Dtype, Length, std::array<Dtype, Length>>
{
public:
    using BaseClass = VectorBase<Dtype, Length, std::array<Dtype, Length>>;

    Vector() : BaseClass() {}

    Vector(Dtype fill_value) : BaseClass(fill_value) {}

    Vector(std::initializer_list<Dtype> list) : BaseClass(list) {}
};

template <typename Dtype>
class Vector<Dtype, Dynamic>
    : public VectorBase<Dtype, Dynamic, UniquePtrWrapper<Dtype>>
{
public:
    using BaseClass = VectorBase<Dtype, Dynamic, UniquePtrWrapper<Dtype>>;

    // Need to use 'this' pointer below because templated base class members are
    // not visible in a certain phase of compilation. See here:
    // http://stackoverflow.com/a/6592617
    Vector(int32_t size) : data_(UniquePtrWrapper<Dtype>(size))
    {
        this->nelements_ = size;
        this->nbytes_ = size * int32_t(sizeof(Dtype));
    }

    Vector(int32_t size, Dtype fill_scalar)
        : data_(UniquePtrWrapper<Dtype>(size))
    {
        this->nelements_ = size;
        this->nbytes_ = size * int32_t(sizeof(Dtype));
        std::fill(std::begin(data_), std::end(data_), fill_scalar);
    }

    Vector(std::initializer_list<Dtype> list)
        : data_(UniquePtrWrapper<Dtype>(list.size()))
    {
        this->nelements_ = list.size();
        this->nbytes_ = list.size() * int32_t(sizeof(Dtype));
        std::copy(std::begin(list), std::end(list), std::begin(data_));
    }

private:
    UniquePtrWrapper<Dtype> data_;
};

/*
// Partial specialization for static Vector type
template <typename Dtype, int32_t Length>
class Vector
{
public:
    Vector() : nelements_(Length), data_() {}

    Vector(std::initializer_list<Dtype> list)
        : nelements_(Length), nbytes_(int32_t(sizeof(Dtype)) * nelements_)
    {
        // std::initializer_list::size() is not marked constexpr, so can't use
        // this
        // static_assert(list.size() == Length, "initializer list size
        // mismatch");
        assert(list.size() == Length && "initializer_list size mismatch");
        std::copy(std::begin(list), std::end(list), std::begin(data_));
    }

    // Fill vector with single scalar (useful for 0, etc)
    Vector(const Dtype scalar)
        : nelements_(Length), nbytes_(int32_t(sizeof(Dtype)) * nelements_)
    {
        std::fill(std::begin(data_), std::end(data_), scalar);
    }

    // Move constructor
    Vector(Vector&& other)
        : nelements_(other.nelements_)
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::move(other.data_))
    {
        other.nelements_ = 0;
        other.nbytes_ = 0;
    }

    // Copy constructor
    Vector(const Vector& other)
        : nelements_(other.nelements_), nbytes_(other.nbytes_)
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
    const Dtype* data(void) const
    {
        return reinterpret_cast<const Dtype*>(data_.data());
    }

    Dtype* data(void) { return reinterpret_cast<Dtype*>(data_.data()); }

    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.data());
    }

    char* as_bytes(void) { return reinterpret_cast<char*>(data_.data()); }

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
    Vector& operator/=(const T scalar)
    {
        assert(scalar != 0 && "divide by zero error");
        for (auto&& d : data_) {
            d /= scalar;
        }
        return *this;
    }

    // Copy-assign
    Vector& operator=(const Vector& other)
    {
        if (this != &other) {
            nelements_ = other.nelements_;
            nbytes_ = other.nbytes_;
            data_.release();
            data_ = std::unique_ptr<Dtype[]>(new Dtype[nelements_]);
            for (int32_t i = 0; i < other.nelements_; ++i) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }

    Vector& operator=(Vector&& other)
    {
        if (this != &other) {
            nelements_ = other.nelements_;
            other.nelements_ = 0;
            nbytes_ = other.nbytes_;
            other.nbytes_ = 0;
            data_ = std::move(other.data_);
        }
        return *this;
    }

    template <typename Functor>
    void apply(Functor f)
    {
        std::transform(
            std::begin(data_), std::end(data_), std::begin(data_), f);
    }

private:
    int32_t nelements_;
    int32_t nbytes_;
    std::array<Dtype, Length> data_;
};
*/

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

/*
// Partial specialization for dynamic Vector type
template <typename Dtype>
class Vector<Dtype, Dynamic>
{
public:
    Vector() : nelements_(0), data_(nullptr) {}

    Vector(const int32_t length)
        : nelements_(int32_t(length))
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::unique_ptr<Dtype[]>(new Dtype[nelements_]()))
    {
    }

    Vector(const int32_t length, const Dtype fill_scalar)
        : nelements_(int32_t(length))
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::unique_ptr<Dtype[]>(new Dtype[nelements_]()))
    {
        fill(fill_scalar);
    }

    // Move constructor
    Vector(Vector&& other)
        : nelements_(other.nelements_)
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::move(other.data_))
    {
        other.nelements_ = 0;
        other.nbytes_ = 0;
        other.data_ = nullptr;
    }

    // Copy constructor
    Vector(const Vector& other)
        : nelements_(other.nelements_), nbytes_(other.nbytes_)
    {
        for (int32_t i = 0; i < other.size(); ++i) {
            data_[i] = other[i];
        }
    }

    void fill(const Dtype scalar)
    {
        std::fill(data_.get(), data_.get() + nelements_, scalar);
    }

    // Const accessor
    const Dtype& operator()(const int32_t i) const { return data_[i]; }

    // Non-const accessor
    Dtype& operator()(const int32_t i) { return data_[i]; }

    // Access elements by single index
    const Dtype& operator[](const int32_t index) const { return data_[index]; }

    Dtype& operator[](int32_t index) { return data_[index]; }

    // Raw accessor for data buffer
    const Dtype* data(void) const
    {
        return reinterpret_cast<const Dtype*>(data_.data());
    }

    Dtype* data(void) { return reinterpret_cast<Dtype*>(data_.data()); }

    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.data());
    }

    char* as_bytes(void) { return reinterpret_cast<char*>(data_.data()); }

    int32_t size(void) const { return nelements_; }

    int32_t size_in_bytes(void) const { return nbytes_; }

    bool empty(void) const { return nelements_ == 0; }

    Dtype max(void) const { return data_[max_element()]; }

    int32_t max_element(void) const
    {
        return std::distance(
            data_.get(),
            std::max_element(data_.get(), data_.get() + nelements_));
    }

    Dtype min(void) const { return data_[min_element()]; }

    int32_t min_element(void) const
    {
        return std::distance(
            data_.get(),
            std::min_element(data_.get(), data_.get() + nelements_));
    }

    // Copy-assign
    Vector& operator=(const Vector& other)
    {
        if (this != &other) {
            nelements_ = other.nelements_;
            nbytes_ = other.nbytes_;
            data_.release();
            data_ = std::unique_ptr<Dtype[]>(new Dtype[nelements_]);
            for (int32_t i = 0; i < other.nelements_; ++i) {
                data_[i] = other.data_[i];
            }
        }
        return *this;
    }

    Vector& operator=(Vector&& other)
    {
        if (this != &other) {
            nelements_ = other.nelements_;
            other.nelements_ = 0;
            nbytes_ = other.nbytes_;
            other.nbytes_ = 0;
            data_ = std::move(other.data_);
        }
        return *this;
    }

    template <typename Functor>
    void apply(Functor f)
    {
        std::transform(data_, data_ + nelements_, data_, f);
    }

    template <typename Functor>
    Vector apply(Functor f) const
    {
        Vector new_v;
        std::transform(data_, data_ + nelements_, new_v.data(), f);
        return new_v;
    }

private:
    int32_t nelements_;
    int32_t nbytes_;
    std::unique_ptr<Dtype[]> data_;
};
*/

template <typename Dtype, int32_t Length>
std::string as_string(const Vector<Dtype, Length>& v)
{
    if (v.empty()) {
        return "[]";
    }
    std::string res = "[";
    for (int32_t i = 1; i < v.size() - 1; ++i) {
        res += std::to_string(v[i]) + ", ";
    }
    res += v[v.size() - 1] + "]";
    return res;
}

// Aliases
// Dynamically-allocated vector
template <typename Dtype>
using VectorX = Vector<Dtype, Dynamic>;
using VectorXd = VectorX<double>;

template <typename T, int32_t Length>
std::ostream& operator<<(std::ostream& s, const Vector<T, Length>& v)
{
    if (v.size() == 0) {
        return s << "[]";
    }
    s << "[";
    for (int32_t i = 0; i < v.size() - 1; ++i) {
        s << std::to_string(v[i]) << ", ";
    }
    return s << std::to_string(v[v.size() - 1]) << "]";
}

template <typename T, typename Scalar>
VectorX<T> operator*(const VectorX<T>& v, const Scalar s)
{
    VectorX<T> new_v(v.size());
    for (int32_t i = 0; i < v.size(); ++i) {
        new_v[i] = v[i] * s;
    }
    return new_v;
}

template <typename T, typename Scalar>
VectorX<T> operator*(const Scalar s, const VectorX<T> v)
{
    return v * s;
}

template <typename T>
VectorXd operator/(const VectorX<T>& v, const double s)
{
    VectorXd new_v(v.size());
    for (int32_t i = 0; i < v.size(); ++i) {
        new_v[i] = v[i] / s;
    }
    return new_v;
}
}

#endif
