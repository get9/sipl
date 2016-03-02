#pragma once

#ifndef SIPL_MATRIX_VECTOR_H
#define SIPL_MATRIX_VECTOR_H

#include <algorithm>
#include <array>
#include "matrix/VectorBase.hpp"
#include "Constants.hpp"

namespace sipl
{

// Wrapper for Dtype*. Need this so we can use the regular VectorBase calls.
template <typename Dtype>
struct DynamicArrayWrapper {
    Dtype* data_;
    int32_t size_;

    DynamicArrayWrapper() : data_(nullptr) {}

    DynamicArrayWrapper(int32_t size) : data_(new Dtype[size]), size_(size) {}

    DynamicArrayWrapper(DynamicArrayWrapper&& other)
        : data_(std::move(other.data_)), size_(other.size_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    DynamicArrayWrapper& operator=(const DynamicArrayWrapper& other)
    {
        if (data_ != nullptr) {
            delete[] data_;
        }
        size_ = other.size_;
        data_ = new Dtype[other.size_];
        std::copy(std::begin(other), std::end(other), data_);
        return *this;
    }

    DynamicArrayWrapper& operator=(DynamicArrayWrapper&& other)
    {
        data_ = std::move(other.data_);
        other.data_ = nullptr;
        size_ = other.size_;
        other.size_ = 0;
        return *this;
    }

    Dtype* begin() { return data_; }

    const Dtype* begin() const { return data_; }

    Dtype* end() { return data_ + size_; }

    const Dtype* end() const { return data_ + size_; }

    int32_t size() const { return size_; }

    Dtype& operator[](int32_t index) { return data_[index]; }

    const Dtype& operator[](int32_t index) const { return data_[index]; }

    Dtype& operator()(int32_t index) { return data_[index]; }

    const Dtype& operator()(int32_t index) const { return data_[index]; }

    Dtype* data() { return data_; }

    const Dtype* data() const { return data_; }
};

// Derived class for statically allocated Vector.
template <typename Dtype, int32_t Length>
class Vector : public VectorBase<Dtype, Length, std::array<Dtype, Length>>
{
public:
    using BaseClass = VectorBase<Dtype, Length, std::array<Dtype, Length>>;
    using BaseClass::data_;

    Vector() : BaseClass() {}

    Vector(Dtype fill_value) : BaseClass(fill_value) {}

    Vector(std::initializer_list<Dtype> list) : BaseClass(list) {}
};

// Specialization of the above for dynamically-allocated Vector.
template <typename Dtype>
class Vector<Dtype, Dynamic>
    : public VectorBase<Dtype, Dynamic, DynamicArrayWrapper<Dtype>>
{
public:
    using BaseClass = VectorBase<Dtype, Dynamic, DynamicArrayWrapper<Dtype>>;

    // Need to use 'this' pointer below because templated base class members are
    // not visible in a certain phase of compilation. See here:
    // http://stackoverflow.com/a/6592617
    Vector(int32_t size)
    {
        this->nelements_ = size;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
    }

    Vector(int32_t size, Dtype fill_value)
    {
        this->nelements_ = size;
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    Vector(std::initializer_list<Dtype> list)
    {
        this->nelements_ = list.size();
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        std::copy(std::begin(list), std::end(list), std::begin(this->data_));
    }
};

// Static vector aliases
template <typename Dtype>
using Vector3 = Vector<Dtype, 3>;
using Vector3b = Vector3<uint8_t>;
using Vector3d = Vector3<double>;
using Vector3f = Vector3<float>;
using Vector3i = Vector3<int32_t>;
using RgbPixel = Vector3b;

// Dynamic vector aliases
template <typename Dtype>
using VectorX = Vector<Dtype, Dynamic>;
using VectorXf = VectorX<float>;
using VectorXd = VectorX<double>;
using VectorXi = VectorX<int32_t>;
}

#endif
