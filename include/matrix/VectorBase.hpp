#pragma once

#ifndef SIPL_MATRIX_VECTORBASE_H
#define SIPL_MATRIX_VECTORBASE_H

#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include "matrix/Common.hpp"

namespace sipl
{

template <typename Dtype, int32_t Length, typename Container>
class VectorBase
{
public:
    using value_type = Dtype;

    // Default constructor
    VectorBase()
        : nelements_(Length), nbytes_(nelements_ * int32_t(sizeof(Dtype)))
    {
    }

    // Initializer list constructor
    VectorBase(std::initializer_list<Dtype> list)
        : nelements_(list.size())
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)) * nelements_)
    {
        // initializer_list::size is not marked constexpr, so can't use
        // static_assert
        assert(list.size() == Length && "initializer_list size mismatch");
        std::copy(std::begin(list), std::end(list), std::begin(data_));
    }

    // Fill vector with single scalar (useful for 0, etc)
    VectorBase(const Dtype scalar)
        : nelements_(Length), nbytes_(int32_t(sizeof(Dtype)) * nelements_)
    {
        std::fill(std::begin(data_), std::end(data_), scalar);
    }

    // Move constructor
    VectorBase(VectorBase&& other)
        : nelements_(other.nelements_)
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(std::move(other.data_))
    {
        other.nelements_ = 0;
        other.nbytes_ = 0;
    }

    // Convenience constructor for unifying static and dynamically-allocated
    // vectors. In the static case, this doesn't actually do anything because
    // the std::array representation is already fixed. In the dynamic case, a
    // new array is allocated of this size anyway.
    /*
    VectorBase(int32_t size)
        : nelements_(size), nbytes_(nelements_ * int32_t(sizeof(Dtype)))
    {
    }
    */

    // Copy constructor
    VectorBase(const VectorBase& other)
        : nelements_(other.nelements_), nbytes_(other.nbytes_)
    {
        std::copy(
            std::begin(other.data_), std::end(other.data_), std::begin(data_));
    }

    // Copy-assign
    VectorBase& operator=(const VectorBase& other)
    {
        if (this != &other) {
            nelements_ = other.nelements_;
            nbytes_ = other.nbytes_;
            std::copy(std::begin(other.data_),
                      std::end(other.data_),
                      std::begin(data_));
        }
        return *this;
    }

    // Move-assign
    VectorBase& operator=(VectorBase&& other)
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

    // Element accessors
    const Dtype& operator()(const int32_t i) const
    {
        assert(index >= 0 && index < nelements_ && "out of range");
        return data_[i];
    }

    Dtype& operator()(const int32_t i)
    {
        assert(index >= 0 && index < nelements_ && "out of range");
        return data_[i];
    }

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

    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.data());
    }

    char* as_bytes(void) { return reinterpret_cast<char*>(data_.data()); }

    int32_t size(void) const { return nelements_; }

    int32_t size_in_bytes(void) const { return nbytes_; }

    // op-then-store operations
    // XXX revisit for better != 0 comparison
    template <typename T>
    VectorBase& operator/=(const T scalar)
    {
        assert(scalar != 0 && "divide by zero error");
        apply([=](Dtype d) { return d / scalar; });
        return *this;
    }

    template <typename T>
    VectorBase& operator*=(const T scalar)
    {
        apply([=](Dtype d) { return d * scalar; });
        return *this;
    }

    template <typename T>
    VectorBase& operator+=(const T scalar)
    {
        apply([=](Dtype d) { return d + scalar; });
        return *this;
    }

    template <typename T>
    VectorBase& operator-=(const T scalar)
    {
        apply([=](Dtype d) { return d - scalar; });
        return *this;
    }

    // Unary minus
    VectorBase operator-() const
    {
        VectorBase new_v(nelements_);
        std::transform(std::begin(data_),
                       std::end(data_),
                       std::begin(new_v),
                       [](Dtype e) { return -e; });
        return new_v;
    }

    // Apply a functor to each element in-place
    template <typename Functor>
    void apply(Functor f)
    {
        std::transform(
            std::begin(data_), std::end(data_), std::begin(data_), f);
    }

    // Apply a functor to each elment returning a new modified vector
    template <typename Functor>
    VectorBase apply(Functor f) const
    {
        VectorBase new_v(nelements_);
        std::transform(
            std::begin(data_), std::end(data_), std::begin(new_v.data()), f);
        return new_v;
    }

    // Mathematical operations
    double norm(NormType type = NormType::L2) const
    {
        switch (type) {
        case NormType::L1:
            return std::sqrt(
                std::accumulate(std::begin(data_),
                                std::end(data_),
                                0.0,
                                [](Dtype e) { return std::abs(e); }));
        case NormType::L2:
            return std::sqrt(std::accumulate(std::begin(data_),
                                             std::end(data_),
                                             0.0,
                                             [](Dtype e) { return e * e; }));
        case NormType::INF:
            return max();
        }
    }

    Dtype max(void) const
    {
        return *std::max_element(std::begin(data_), std::end(data_));
    }

    Dtype min(void) const
    {
        return *std::min_element(std::begin(data_), std::end(data_));
    }

    int32_t argmax() const
    {
        auto m = std::max_element(std::begin(data_), std::end(data_));
        return m - std::begin(data_);
    }

    int32_t argmin() const
    {
        auto m = std::min_element(std::begin(data_), std::end(data_));
        return m - std::begin(data_);
    }

protected:
    int32_t nelements_;
    int32_t nbytes_;
    Container data_;
};
}

#endif
