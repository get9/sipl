#pragma once

#ifndef SIPL_MATRIX_WRAPPERS_HPP
#define SIPL_MATRIX_WRAPPERS_HPP

#include <algorithm>

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
        data_ = new Dtype[other.size_];
        size_ = other.size_;
        std::copy(std::begin(other), std::end(other), data_);
        return *this;
    }

    DynamicArrayWrapper& operator=(DynamicArrayWrapper&& other)
    {
        data_ = std::move(other.data_);
        size_ = other.size_;
        other.data_ = nullptr;
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

    Dtype& front()
    {
        assert(size_ > 0 && "size mismatch");
        return data_[0];
    }

    const Dtype& front() const
    {
        assert(size_ > 0 && "size mismatch");
        return data_[0];
    }

    Dtype& back()
    {
        assert(size_ > 0 && "size mismatch");
        return data_[size_ - 1];
    }

    const Dtype& back() const
    {
        assert(size_ > 0 && "size mismatch");
        return data_[size_ - 1];
    }
};

// Wrapper for Dtype*. Need this so we can use the regular VectorBase calls.
template <typename Dtype, int32_t Length>
struct StaticArrayWrapper {
    std::array<Dtype, Length> data_;
    int32_t size_;

    StaticArrayWrapper() : data_(), size_(0) {}

    StaticArrayWrapper(int32_t size) : data_(), size_(size) {}

    StaticArrayWrapper(StaticArrayWrapper&& other)
        : data_(std::move(other.data_)), size_(other.size_)
    {
        other.size_ = 0;
    }

    StaticArrayWrapper& operator=(const StaticArrayWrapper& other)
    {
        size_ = other.size_;
        std::copy(std::begin(other), std::end(other), std::begin(data_));
        return *this;
    }

    StaticArrayWrapper& operator=(StaticArrayWrapper&& other)
    {
        data_ = std::move(other.data_);
        size_ = other.size_;
        other.size_ = 0;
        return *this;
    }

    Dtype* begin() { return data_.begin(); }
    const Dtype* begin() const { return data_.begin(); }

    Dtype* end() { return data_.end(); }
    const Dtype* end() const { return data_.end(); }

    int32_t size() const { return size_; }

    Dtype& operator[](int32_t index) { return data_[index]; }
    const Dtype& operator[](int32_t index) const { return data_[index]; }

    Dtype& operator()(int32_t index) { return data_[index]; }
    const Dtype& operator()(int32_t index) const { return data_[index]; }

    Dtype* data() { return data_.data(); }
    const Dtype* data() const { return data_.data(); }

    Dtype& front()
    {
        assert(size_ > 0 && "size mismatch");
        return data_[0];
    }
    const Dtype& front() const
    {
        assert(size_ > 0 && "size mismatch");
        return data_[0];
    }

    Dtype& back()
    {
        assert(size_ > 0 && "size mismatch");
        return data_[size_ - 1];
    }
    const Dtype& back() const
    {
        assert(size_ > 0 && "size mismatch");
        return data_[size_ - 1];
    }
};

#endif
