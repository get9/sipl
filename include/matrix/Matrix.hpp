#pragma once

#ifndef _SIPL_MATRIX_H_
#define _SIPL_MATRIX_H_

#include <initializer_list>
#include <functional>
#include <numeric>
#include <vector>
#include <memory>

namespace sipl
{
template <typename Dtype>
class Matrix
{
public:
    // Default constructor, no data
    Matrix() : data_(nullptr), dims_(), nelements_(0) {}
    // Sized constructor. Dims stored in initializer list and initializes memory
    Matrix(std::initializer_list<size_t> dims)
        : dims_(dims),
          nelements_(std::accumulate(std::begin(dims), std::end(dims), 1,
                                     std::multiplies<size_t>())),
          data_(std::unique_ptr<Dtype[]>(new Dtype[sizeof(Dtype) * nelements_]))
    {
    }

    // Const accessor
    const Dtype& operator()(std::vector<size_t> idxs) const
    {
        return data_[calculate_index(idxs)];
    }

    // Non-const accessor
    Dtype& operator()(const std::vector<size_t>& idxs)
    {
        return data_[calculate_index(idxs)];
    }

    // Access elements by single index
    const Dtype& operator[](size_t index) const { return data_[index]; }
    Dtype& operator[](size_t index) { return data_[index]; }
    // Raw accessor for data buffer
    Dtype* data(void)
    {
        Dtype* raw_ptr = data_.get();
        return raw_ptr;
    }

    size_t size(void) const { return nelements_; }
    std::vector<size_t> dims(void) const
    {
        std::vector<size_t> ds;
        ds.reserve(dims_.size());
        for (const auto d : dims_) {
            ds.push_back(d);
        }
        return std::move(ds);
    }

private:
    std::vector<size_t> dims_;
    size_t nelements_;
    std::unique_ptr<Dtype[]> data_;

    size_t calculate_index(const std::vector<size_t>& idxs) const
    {
        assert(idxs.size() == dims_.size() &&
               "idxs and dims_ must be same size");
        size_t target_index = 0;
        for (int32_t i = 0; i < dims_.size(); ++i) {
            target_index += idxs[i] * dims_[i];
        }
        return target_index;
    }
};
}

#endif
