#pragma once

#ifndef SIPL_ARRAY_STATIC_ARRAY_H
#define SIPL_ARRAY_STATIC_ARRAY_H

#include <memory>
#include <array>
#include "array/util.hpp"

namespace sipl
{

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
};
}

#endif
