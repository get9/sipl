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

template <typename T>
T clamp(double val)
{
    const auto min = std::numeric_limits<T>::min();
    const auto max = std::numeric_limits<T>::max();
    if (val > max) {
        return max;
    } else if (val < min) {
        return min;
    } else {
        return T(std::round(val));
    }
}

// Overload for double vals so they don't get rounded
double clamp(double val) { return val; }

// Overload for Vector3d types
RgbPixel clamp(const Vector3d& v)
{
    return v.apply([](auto e) { return clamp(e); });
}

template <typename T>
Vector3<T> homogenize(const Vector3<T>& v)
{
    return v / v[2];
}
}

#endif
