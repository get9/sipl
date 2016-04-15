#pragma once

#ifndef SIPL_MATRIX_VECTOR_H
#define SIPL_MATRIX_VECTOR_H

#include "Constants.hpp"
#include "matrix/VectorBase.hpp"
#include "matrix/Wrappers.hpp"
#include <algorithm>
#include <array>
#include <functional>

namespace sipl
{

// Derived class for statically allocated Vector.
template <typename Dtype, int32_t Length>
class Vector
    : public VectorBase<Dtype, Length, StaticArrayWrapper<Dtype, Length>>
{
public:
    using ContainerType = StaticArrayWrapper<Dtype, Length>;
    using BaseClass = VectorBase<Dtype, Length, ContainerType>;
    using BaseClass::BaseClass;

    // Explicitly import base class members so we don't have to use 'this'
    // pointer to refer to them (see here for a detailed explanation:
    // http://stackoverflow.com/a/4643295)
    using BaseClass::nelements_;
    using BaseClass::nbytes_;
    using BaseClass::data_;

    Vector() : BaseClass() {}

    Vector(Dtype fill_value) : BaseClass()
    {
        std::fill(std::begin(data_), std::end(data_), fill_value);
    }

    template <typename OtherType>
    Vector<OtherType, Length> as_type() const
    {
        return apply([](auto e) { return OtherType(e); });
    }

    template <typename OtherType>
    Vector(const Vector<OtherType, Length>& other)
    {
        nelements_ = other.nelements_;
        nbytes_ = other.nbytes_;
        std::transform(std::begin(other), std::end(other), std::begin(data_),
                       [](auto e) { return OtherType(e); });
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType =
                  typename std::result_of<UnaryFunctor&(Dtype)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Vector<OutputType, Length> new_v;
        std::transform(std::begin(*this), std::end(*this), std::begin(new_v),
                       f);
        return new_v;
    }

    template <typename OtherType>
    auto dot(const Vector<OtherType, Length>& other) const
        -> decltype(this->front() * other.front())
    {
        Vector<decltype(this->front() * other.front()), Length> new_v;
        for (int32_t i = 0; i < other.size(); ++i) {
            new_v[i] = (*this)[i] * other[i];
        }
        return new_v.sum();
    }
};

// Specialization of the above for dynamically-allocated Vector.
template <typename Dtype>
class Vector<Dtype, Dynamic>
    : public VectorBase<Dtype, Dynamic, DynamicArrayWrapper<Dtype, Dynamic>>
{
public:
    using ContainerType = DynamicArrayWrapper<Dtype, Dynamic>;
    using BaseClass = VectorBase<Dtype, Dynamic, ContainerType>;
    using BaseClass::BaseClass;

    using BaseClass::nelements_;
    using BaseClass::nbytes_;
    using BaseClass::data_;

    Vector() = delete;

    // Regular constructor for dynamically sized Vector
    Vector(int32_t size)
    {
        nelements_ = size;
        nbytes_ = nelements_ * int32_t(sizeof(Dtype));
        data_ = ContainerType(nelements_);
    }

    // Construct + fill
    Vector(int32_t size, Dtype fill_value)
    {
        nelements_ = size;
        data_ = ContainerType(nelements_);
        nbytes_ = nelements_ * int32_t(sizeof(Dtype));
        std::fill(std::begin(data_), std::end(data_), fill_value);
    }

    template <typename OtherType>
    Vector<OtherType, Dynamic> as_type() const
    {
        // Note: include std::round so we round floating point types to the
        // nearest integral type
        return apply([](auto e) { return OtherType(std::round(e)); });
    }

    template <typename OtherType>
    auto dot(const Vector<OtherType, Dynamic>& other) const
        -> decltype(this->front() * other.front())
    {
        Vector<decltype(this->front() * other.front()), Dynamic> new_v(
            other.size());
        for (int32_t i = 0; i < other.size(); ++i) {
            new_v[i] = (*this)[i] * other[i];
        }
        return new_v.sum();
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType =
                  typename std::result_of<UnaryFunctor&(Dtype)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Vector<OutputType, Dynamic> new_v(nelements_);
        std::transform(std::begin(*this), std::end(*this), std::begin(new_v),
                       f);
        return new_v;
    }
};

// Static vector aliases
template <typename Dtype>
using Vector2 = Vector<Dtype, 2>;
using Vector2b = Vector2<uint8_t>;
using Vector2d = Vector2<double>;
using Vector2f = Vector2<float>;
using Vector2i = Vector2<int32_t>;

template <typename Dtype>
using Vector3 = Vector<Dtype, 3>;
using Vector3b = Vector3<uint8_t>;
using Vector3d = Vector3<double>;
using Vector3f = Vector3<float>;
using Vector3i = Vector3<int32_t>;
using RgbPixel = Vector3b;

template <typename Dtype>
using Vector4 = Vector<Dtype, 4>;
using Vector4d = Vector4<double>;
using Vector4i = Vector4<int32_t>;
using Vector4b = Vector4<uint8_t>;

// Dynamic vector aliases
template <typename Dtype>
using VectorX = Vector<Dtype, Dynamic>;
using VectorXf = VectorX<float>;
using VectorXd = VectorX<double>;
using VectorXi = VectorX<int32_t>;
}

#endif
