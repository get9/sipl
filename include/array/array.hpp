#pragma once

#ifndef SIPL_ARRAY_ARRAY_H
#define SIPL_ARRAY_ARRAY_H

#include <memory>
#include <vector>
#include "array/util.hpp"

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
    }

    size_t size() const { return size_; };

private:
    size_t size_;
    std::shared_ptr<Dtype> buffer_;
    std::vector<size_t> shape_;
};
}

#endif
