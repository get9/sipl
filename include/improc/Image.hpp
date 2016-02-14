#pragma once

#ifndef SIPL_IMPROC_IMAGE_H
#define SIPL_IMPROC_IMAGE_H

#include "matrix/Matrix.hpp"

namespace sipl
{
template <typename T>
class Image
{
public:
    const int32_t width;
    const int32_t height;

    Image() : width(0), height(0) {}

    Image(MatrixX<T> mat) : data_(mat), width(m.dims[1]), height(m.dims[0]) {}

    const T& operator()(int32_t x, int32_t y) const { return data_(y, x); }

    T& operator()(int32_t x, int32_t y) { return data_(y, x); }

private:
    MatrixX<T> data_;
};
}

#endif
