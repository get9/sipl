#pragma once

#ifndef SIPL_MATRIX_VECTOR_H
#define SIPL_MATRIX_VECTOR_H

#include <algorithm>
#include <array>
#include "matrix/Wrappers.hpp"
#include "matrix/VectorBase.hpp"
#include "Constants.hpp"

namespace sipl
{

// Derived class for statically allocated Vector.
template <typename Dtype, int32_t Length>
class Vector
    : public VectorBase<Dtype, Length, StaticArrayWrapper<Dtype, Length>>
{
public:
    using BaseClass =
        VectorBase<Dtype, Length, StaticArrayWrapper<Dtype, Length>>;
    using BaseClass::BaseClass;
    /*
    Vector()
    {
        this->nelements_ = 0;
        this->nbytes_ = 0;
        this->data_ = StaticArrayWrapper<Dtype, Length>();
    }

    Vector(Dtype fill_value)
    {
        this->nelements_ = Length;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Length>(this->nelements_);
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    Vector(std::initializer_list<Dtype> list)
    {
        this->nelements_ = list.size();
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Length>(this->nelements_);
        std::copy(std::begin(list), std::end(list), std::begin(this->data_));
    }

    Vector(const Vector& other)
    {
        this->nelements_ = Length;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Length>(this->nelements_);
        std::copy(std::begin(other.data_),
                  std::end(other.data_),
                  std::begin(this->data_));
    }

    Vector(Vector&& other)
    {
        this->nelements_ = other.nelements_;
        this->nbytes_ = other.nbytes_;
        this->data_ = std::move(other.data_);
    }

    template <typename OtherType>
    Vector(const Vector<OtherType, Length>& v)
    {
        this->nelements_ = v.size();
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Length>(this->nelements_);
        std::transform(std::begin(v.data()),
                       std::end(v.data()),
                       std::begin(this->data_),
                       [](auto e) { return Dtype(e); });
    }

    // Copy-assign
    Vector& operator=(const Vector& other)
    {
        if (this != &other) {
            this->nelements_ = other.nelements_;
            this->nbytes_ = other.nbytes_;
            std::copy(std::begin(other.data_),
                      std::end(other.data_),
                      std::begin(this->data_));
        }
        return *this;
    }

    // Move-assign
    Vector& operator=(Vector&& other)
    {
        if (this != &other) {
            this->nelements_ = other.nelements_;
            this->nbytes_ = other.nbytes_;
            this->data_ = std::move(other.data_);
        }
        return *this;
    }
*/
    Vector(Dtype fill_value)
    {
        this->nelements_ = Length;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = StaticArrayWrapper<Dtype, Length>(this->nelements_);
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }
};

// Specialization of the above for dynamically-allocated Vector.
template <typename Dtype>
class Vector<Dtype, Dynamic>
    : public VectorBase<Dtype, Dynamic, DynamicArrayWrapper<Dtype, Dynamic>>
{
public:
    using BaseClass =
        VectorBase<Dtype, Dynamic, DynamicArrayWrapper<Dtype, Dynamic>>;
    using BaseClass::BaseClass;
    // Need to use 'this' pointer below because templated base class members are
    // not visible in a certain phase of compilation. See here:
    // http://stackoverflow.com/a/6592617
    Vector(int32_t size)
    {
        this->nelements_ = size;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = DynamicArrayWrapper<Dtype, Dynamic>(this->nelements_);
        this->data_.size_ = size;
    }

    Vector(int32_t size, Dtype fill_value)
    {
        this->nelements_ = size;
        this->data_ = DynamicArrayWrapper<Dtype, Dynamic>(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
        this->data_.size_ = size;
    }
    /*

    Vector(std::initializer_list<Dtype> list)
    {
        this->nelements_ = list.size();
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        std::copy(std::begin(list), std::end(list), std::begin(this->data_));
    }

    Vector(const Vector& other)
    {
        this->nelements_ = other.nelements_;
        this->nbytes_ = other.nbytes_;
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
        std::copy(std::begin(other.data_),
                  std::end(other.data_),
                  std::begin(this->data_));
    }

    // Convert constructor
    template <typename OtherType>
    Vector(const Vector<OtherType, Dynamic>& v)
    {
        this->nelements_ = v.size();
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = DynamicArrayWrapper<Dtype>(this->nelements_);
        std::transform(std::begin(v),
                       std::end(v),
                       std::begin(this->data_),
                       [](auto e) { return Dtype(e); });
    }

    Vector(Vector&& other)
    {
        this->nelements_ = other.nelements_;
        this->nbytes_ = other.nbytes_;
        this->data_ = std::move(other.data_);
    }

    // Copy-assign
    Vector& operator=(const Vector& other)
    {
        if (this != &other) {
            this->nelements_ = other.nelements_;
            this->nbytes_ = other.nbytes_;
            std::copy(std::begin(other.data_),
                      std::end(other.data_),
                      std::begin(this->data_));
        }
        return *this;
    }

    // Move-assign
    Vector& operator=(Vector&& other)
    {
        if (this != &other) {
            this->nelements_ = other.nelements_;
            this->nbytes_ = other.nbytes_;
            this->data_ = std::move(other.data_);
        }
        return *this;
    }
    */
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
