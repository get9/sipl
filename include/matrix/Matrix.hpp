#pragma once

#ifndef SIPL_MATRIX_H
#define SIPL_MATRIX_H

#include <functional>
#include <numeric>
#include <array>
#include <memory>
#include <iostream>

namespace sipl
{
template <typename Dtype, uint32_t Dims>
class Matrix
{
public:
    const std::array<size_t, Dims> dims;

    // Default constructor, no data
    Matrix() : dims(), nelements_(0), data_(nullptr) {}
    // Sized constructor. Dims stored in initializer list and initializes
    // nmemory
    Matrix(std::array<size_t, Dims> ds)
        : dims(ds),
          nelements_(std::accumulate(std::begin(dims), std::end(dims),
                                     size_t(1), std::multiplies<size_t>())),
          data_(std::unique_ptr<Dtype[]>(new Dtype[sizeof(Dtype) * nelements_]))
    {
    }

    // Const accessor
    const Dtype& operator()(const std::array<size_t, Dims>& idxs) const
    {
        return data_[calculate_index(idxs)];
    }

    // Non-const accessor
    Dtype& operator()(const std::array<size_t, Dims>& idxs)
    {
        return data_[calculate_index(idxs)];
    }

    // Access elements by single index
    const Dtype& operator[](size_t index) const { return data_[index]; }
    Dtype& operator[](size_t index) { return data_[index]; }
    // Raw accessor for data buffer
    const Dtype* buffer(void) const
    {
        return reinterpret_cast<const Dtype*>(data_.get());
    }
    Dtype* buffer(void) { return reinterpret_cast<Dtype*>(data_.get()); }
    // Accessors for the buffer as bytes (for serialization, etc)
    const char* as_bytes(void) const
    {
        return reinterpret_cast<const char*>(data_.get());
    }
    char* bytes(void) { return reinterpret_cast<char*>(data_.get()); }
    size_t size(void) const { return nelements_; }
    size_t size_in_bytes(void) const { return nelements_ * sizeof(Dtype); }
private:
    size_t nelements_;
    std::unique_ptr<Dtype[]> data_;

    size_t calculate_index(const std::array<size_t, Dims>& idxs) const
    {
        size_t target_index = 0;
        for (size_t i = 0; i < dims.size() - 1; ++i) {
            target_index += idxs[i] * dims[i + 1];
        }
        return target_index + idxs.back();
    }
};
}

#endif
