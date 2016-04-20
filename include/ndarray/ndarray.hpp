#pragma once

#ifndef SIPL_NDARRAY_NDARRAY_H
#define SIPL_NDARRAY_NDARRAY_H

#include <memory>
#include <vector>
#include "ndarray/util.hpp"

namespace sipl
{

template <typename Dtype>
class array
{
public:
    array() : size_(0), buffer_(nullptr) {}

    template <typename... Shape>
    array(Shape... shape)
        : size_(util::multiply(shape...))
        , buffer_(new Dtype[size_], std::default_delete<Dtype[]>())
        , shape_()
    {
        std::forward_as_tuple(shape...)
    }

    size_t size() const { return size_; };

private:
    size_t size_;
    std::shared_ptr<Dtype> buffer_;
    std::vector<size_t> shape_;
};

template <typename Dtype, size_t... Shape>
class static_array
{
public:
    static_array()
        : size_(util::multiply<Shape...>())
        , buffer_(new Dtype[size_], std::default_delete<Dtype[]>())
    {
    }

private:
    size_t size_;
    std::shared_ptr<Dtype> buffer_;
    std::array<size_t, sizeof...(Shape)> shape_;
}
}

#endif
