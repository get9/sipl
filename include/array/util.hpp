#pragma once

#ifndef SIPL_NDARRAY_UTIL_H
#define SIPL_NDARRAY_UTIL_H

#include <tuple>

namespace sipl
{

namespace util
{

template <typename T>
T multiply(T v)
{
    return v;
}

template <typename T, typename... Args>
T multiply(T first, Args... args)
{
    return first * multiply(args...);
}
}
}

#endif
