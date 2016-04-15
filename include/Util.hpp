#pragma once

#ifndef SIPL_UTIL_H
#define SIPL_UTIL_H

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

namespace sipl
{

namespace util
{

inline double rad2deg(double rad) { return rad * 180.0 / M_PI; }

inline double deg2rad(double deg) { return deg * M_PI / 180.0; }
}
}

template <typename T>
std::ostream& operator<<(std::ostream& s, const std::vector<T>& v)
{
    std::copy(std::begin(v), std::end(v), std::ostream_iterator<T>(s, "\n"));
    return s;
}

#endif
