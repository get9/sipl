#pragma once

#ifndef SIPL_COMMON_H
#define SIPL_COMMON_H

#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include "matrix/Vector"

namespace sipl
{

// Clamp a given value to the specified type
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

// Overload for vector types
template <typename T, int32_t Length>
Vector<T, Length> clamp(const Vector<double, Length>& v)
{
    return v.apply([](auto e) { return clamp<T>(e); });
}

// Function specialization for clamping doubles (to ensure they don't get
// rounded)
template <>
inline double clamp<double>(double val)
{
    return val;
}
template <>
inline float clamp<float>(double val)
{
    return float(val);
}

// Overload for double vals so they don't get rounded
template <int32_t Length>
Vector<double, Length> clamp(const Vector<double, Length>& v)
{
    return v;
}

inline double deg2rad(double degrees) { return degrees * M_PI / 180.0; }

inline double rad2deg(double radians) { return radians * 180.0 / M_PI; }
}

#endif
