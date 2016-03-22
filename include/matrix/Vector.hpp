#pragma once

#ifndef SIPL_MATRIX_VECTOR_H
#define SIPL_MATRIX_VECTOR_H

#include <algorithm>
#include <functional>
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
    using ContainerType = StaticArrayWrapper<Dtype, Length>;
    using BaseClass = VectorBase<Dtype, Length, ContainerType>;
    using BaseClass::BaseClass;

    Vector()
    {
        this->nelements_ = Length;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType();
    }

    Vector(Dtype fill_value)
    {
        this->nelements_ = Length;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType();
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    template <typename OtherType>
    Vector<OtherType, Length> as_type() const
    {
        return apply([](auto e) { return OtherType(std::round(e)); });
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType =
                  typename std::result_of<UnaryFunctor&(Dtype)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Vector<OutputType, Length> new_v;
        std::transform(this->begin(), this->end(), std::begin(new_v), f);
        return new_v;
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

    Vector() = delete;

    // Need to use 'this' pointer below because templated base class members are
    // not visible in a certain phase of compilation. See here:
    // http://stackoverflow.com/a/6592617
    Vector(int32_t size)
    {
        this->nelements_ = size;
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        this->data_ = ContainerType(this->nelements_);
    }

    Vector(int32_t size, Dtype fill_value)
    {
        this->nelements_ = size;
        this->data_ = ContainerType(this->nelements_);
        this->nbytes_ = this->nelements_ * int32_t(sizeof(Dtype));
        std::fill(std::begin(this->data_), std::end(this->data_), fill_value);
    }

    template <typename OtherType>
    Vector<OtherType, Dynamic> as_type() const
    {
        return apply([](auto e) { return OtherType(std::round(e)); });
    }

    // Template magic from: http://stackoverflow.com/a/26383814
    template <typename UnaryFunctor,
              typename OutputType =
                  typename std::result_of<UnaryFunctor&(Dtype)>::type>
    decltype(auto) apply(UnaryFunctor f) const
    {
        Vector<OutputType, Dynamic> new_v(this->nelements_);
        std::transform(this->begin(), this->end(), std::begin(new_v), f);
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
