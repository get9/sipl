#pragma once

#ifndef SIPL_IMPROC_UTILITY_H
#define SIPL_IMPROC_UTILITY_H

#include <limits>
#include <cmath>
#include "matrix/Vector"

namespace sipl
{

// Clamp a value to min/max values
uint8_t clamp(const double val)
{
    const auto min = std::numeric_limits<uint8_t>::min();
    const auto max = std::numeric_limits<uint8_t>::max();
    if (val >= max) {
        return max;
    } else if (val <= min) {
        return min;
    } else {
        return uint8_t(std::round(val));
    }
}

RgbPixel clamp(const Vector3d& in_vec)
{
    RgbPixel out_vec;
    out_vec[0] = clamp(in_vec[0]);
    out_vec[1] = clamp(in_vec[1]);
    out_vec[2] = clamp(in_vec[2]);
    return out_vec;
}

template <typename T>
Vector3<T> homogenize(const Vector3<T>& v)
{
    return v / v[2];
}
}

#endif
