#pragma once

#ifndef _SIPL_MATRIX_H_
#define _SIPL_MATRIX_H_

#include <initializer_list>
#include <functional>
#include <vector>
#include <memory>

namespace sipl
{
template <typename Dtype>
class Matrix
{
public:
    // Default constructor, no data
    Matrix() : data_(nullptr), dims_(nullptr), nelements_(0) {}
    // Sized constructor. Dims stored in initializer list and initializes memory
    Matrix(std::initializer_list<size_t> dims)
        : dims_(dims),
          nelements_(std::accumulate(std::begin(dims), std::end(dims), 1,
                                     std::multiplies<size_t>())),
          data_(std::make_shared(sizeof(Dtype) * nelements_))
    {
    }

    // Const accessor
    const Dtype& operator()(std::initializer_list<size_t> idxs) const
    {
        return data_[calculate_index(idxs)];
    }

    // Non-const accessor
    Dtype& operator()(const std::initializer_list<size_t>& idxs)
    {
        return data_[calculate_index(idxs)];
    }

    // Accessors
    const Dtype* data(void) const { return data_.get(); }
    Dtype* data(void) { return data_.get(); }
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
    std::shared_ptr<Dtype[]> data_;
    std::initializer_list<size_t> dims_;
    size_t nelements_;

    size_t calculate_index(const std::initializer_list<size_t>& idxs) const
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
