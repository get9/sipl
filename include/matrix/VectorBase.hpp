#pragma once

#ifndef SIPL_MATRIX_VECTORBASE_H
#define SIPL_MATRIX_VECTORBASE_H

#include <algorithm>
#include <numeric>
#include <sstream>
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

    // All derived constructors call this, but we need to make sure the dynamic
    // vector constructors overwrite the values for member vars
    VectorBase()
        : nelements_(Length)
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_()
    {
    }

    VectorBase(std::initializer_list<Dtype> list)
        : nelements_(int32_t(list.size()))
        , nbytes_(nelements_ * int32_t(sizeof(Dtype)))
        , data_(nelements_)
    {
        std::copy(std::begin(list), std::end(list), std::begin(data_));
    }

    template <typename OtherType>
    VectorBase(const VectorBase<OtherType, Length, Container>& other)
        : nelements_(other.nelements_)
        , nbytes_(other.nbytes_)
        , data_(other.nelements_)
    {
        std::transform(std::begin(other), std::end(other), std::begin(data_),
                       [](auto e) { return Dtype(e); });
    }

    VectorBase(const VectorBase& other)
        : nelements_(other.nelements_)
        , nbytes_(other.nbytes_)
        , data_(other.data_)
    {
    }

    VectorBase(VectorBase&& other)
        : nelements_(other.nelements_)
        , nbytes_(other.nbytes_)
        , data_(std::move(other.data_))
    {
    }

    // Copy-assign
    VectorBase& operator=(const VectorBase& other)
    {
        if (this != &other) {
            nelements_ = other.nelements_;
            nbytes_ = other.nbytes_;
            std::copy(std::begin(other.data_), std::end(other.data_),
                      std::begin(data_));
        }
        return *this;
    }

    // Move-assign
    VectorBase& operator=(VectorBase&& other)
    {
        if (this != &other) {
            nelements_ = other.nelements_;
            nbytes_ = other.nbytes_;
            data_ = std::move(other.data_);
        }
        return *this;
    }

    // Convert to a different type
    template <typename OtherType>
    VectorBase<OtherType, Length, Container> as_type() const
    {
        return apply([](auto e) { return OtherType(e); });
    }

    bool empty() const { return nelements_ == 0; }

    // Iterator & element access
    Dtype* begin() { return std::begin(data_); }
    const Dtype* begin() const { return std::begin(data_); }

    Dtype* end() { return std::end(data_); }
    const Dtype* end() const { return std::end(data_); }

    Dtype& front() { return data_.front(); }
    const Dtype& front() const { return data_.front(); }

    Dtype& back() { return data_.back(); }
    const Dtype& back() const { return data_.back(); }

    // Element accessors
    const Dtype& operator()(const int32_t i) const
    {
        assert(i >= 0 && i < nelements_ && "out of range");
        return data_[i];
    }
    Dtype& operator()(const int32_t i)
    {
        assert(i >= 0 && i < nelements_ && "out of range");
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
    VectorBase& operator/=(T scalar)
    {
        assert(scalar != 0 && "divide by zero error");
        transform([=](Dtype d) { return d / double(scalar); });
        return *this;
    }

    template <typename T>
    VectorBase& operator*=(T scalar)
    {
        transform([=](Dtype d) { return d * scalar; });
        return *this;
    }

    template <typename T>
    VectorBase& operator+=(T scalar)
    {
        transform([=](Dtype d) { return d + scalar; });
        return *this;
    }

    template <typename T>
    VectorBase& operator-=(T scalar)
    {
        transform([=](Dtype d) { return d - scalar; });
        return *this;
    }

    // Unary minus
    VectorBase operator-() const
    {
        return apply([](Dtype e) { return -e; });
    }

    // Mathematical operations
    double norm(NormType type = NormType::L2) const
    {
        switch (type) {
        case NormType::L1:
            return std::sqrt(std::accumulate(
                std::begin(data_), std::end(data_), 0.0,
                [](double acc, Dtype e) { return acc + std::abs(e); }));
        case NormType::L2:
            return std::sqrt(std::accumulate(
                std::begin(data_), std::end(data_), 0.0,
                [](double acc, Dtype e) { return acc + e * e; }));
        case NormType::INF:
            return max();
        }
    }

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

    // Convert to a string representation
    std::string str() const
    {
        if (empty()) {
            return "[]";
        }
        std::stringstream ss;
        ss << "[";
        for (auto it = std::begin(data_); it != std::end(data_) - 1; ++it) {
            ss << std::to_string(*it) << ", ";
        }
        ss << std::to_string(back()) << "]";
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& s, const VectorBase& v)
    {
        return s << v.str();
    }

protected:
    int32_t nelements_;
    int32_t nbytes_;
    Container data_;
};
}

#endif
