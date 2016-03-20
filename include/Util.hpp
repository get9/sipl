#pragma once

#ifndef SIPL_UTIL_H
#define SIPL_UTIL_H

#include <limits>

namespace sipl
{

namespace util
{
inline double rad2deg(double rad) { return rad * 180.0 / M_PI; }

inline double deg2rad(double deg) { return deg * M_PI / 180.0; }
}
}

#endif
