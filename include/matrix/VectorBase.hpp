#pragma once

#ifndef SIPL_MATRIX_VECTORBASE_H
#define SIPL_MATRIX_VECTORBASE_H

namespace sipl
{

template <typename Dtype, int32_t Length, typename Container>
class VectorBase
{
public:
    using value_type = Dtype;

    VectorBase()
        : nelements_(Length), nbytes_(Length * int32_t(sizeof(Dtype))), data_()
    {
    }

    VectorBase(std::initializer_list<Dtype> list)
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

    // Copy constructor
    VectorBase(const VectorBase& other)
        : nelements_(other.nelements_), nbytes_(other.nbytes_)
    {
        std::copy(
            std::begin(other.data_), std::end(other.data_), std::begin(data_));
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
    VectorBase& operator/=(const T scalar)
    {
        assert(scalar != 0 && "divide by zero error");
        for (auto&& d : data_) {
            d /= scalar;
        }
        return *this;
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

    template <typename Functor>
    void apply(Functor f)
    {
        std::transform(
            std::begin(data_), std::end(data_), std::begin(data_), f);
    }

protected:
    int32_t nelements_;
    int32_t nbytes_;
    Container data_;
};
}

#endif
