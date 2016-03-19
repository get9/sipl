#pragma once

#ifndef SIPL_UTIL_H
#define SIPL_UTIL_H

#include <limits>

namespace sipl
{

namespace util
{

// Easy min/max access
template <typename T>
constexpr T min = std::numeric_limits<T>::min();

template <typename T>
constexpr T max = std::numeric_limits<T>::max();
}
}

#endif
