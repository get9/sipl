#pragma once

#ifndef SIPL_IMPROC_IMPROC_H
#define SIPL_IMPROC_IMPROC_H

#include <limits>
#include "matrix/Vector.hpp"
#include "matrix/Matrix.hpp"

namespace sipl
{

template <typename T, typename U = double>
T clamp(const U u)
{
    auto min = std::numeric_limits<T>::min();
    auto max = std::numeric_limits<T>::max();
    if (u >= max) {
        return max;
    } else if (u <= min) {
        return min;
    } else {
        return T(std::round(u));
    }
}

Matrix<uint8_t> color_to_grayscale(const Matrix<RGBPixel>& color);
}

#endif
