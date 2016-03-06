#pragma once

#ifndef SIPL_IMPROC_UTILITY_H
#define SIPL_IMPROC_UTILITY_H

#include <limits>
#include <cmath>
#include "matrix/Vector"

namespace sipl
{

// Available kinds of interpolators
enum class InterpolateType { BILINEAR, NEAREST_NEIGHBOR, UNKNOWN };

// Clamp a value to min/max values
uint8_t clamp(double val)
{
    const auto min = std::numeric_limits<uint8_t>::min();
    const auto max = std::numeric_limits<uint8_t>::max();
    if (val > max) {
        return max;
    } else if (val < min) {
        return min;
    } else {
        return uint8_t(std::round(val));
    }
}

RgbPixel clamp(const Vector3d& v)
{
    return {clamp(v[0]), clamp(v[1]), clamp(v[2])};
}

template <typename T>
Vector3<T> homogenize(const Vector3<T>& v)
{
    return v / v[2];
}
}

#endif
