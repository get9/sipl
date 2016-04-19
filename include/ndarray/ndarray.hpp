#pragma once

#ifndef SIPL_NDARRAY_NDARRAY_H
#define SIPL_NDARRAY_NDARRAY_H

#include <memory>
#include "ndarray/util.hpp"

namespace sipl
{

template <typename Dtype>
class ndarray
{
public:
    ndarray() : size_(0), buffer_(nullptr) {}

    template <typename... Shape>
    ndarray(Shape... shape)
        : size_(util::multiply(shape...))
        , buffer_(new Dtype[size_], std::default_delete<Dtype[]>())
    {
    }

    size_t size() const { return size_; };

private:
    size_t size_;
    std::shared_ptr<Dtype> buffer_;
};
}

#endif
