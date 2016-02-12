#pragma once

#ifndef SIPL_MATRIX_VECTOR_H
#define SIPL_MATRIX_VECTOR_H

#include <array>
#include <ostream>
#include <algorithm>

namespace sipl
{

// Helpful defines for vector types
template <typename T, size_t Length>
using Vector = std::array<T, Length>;

template <typename T>
using Vector3 = Vector<T, 3>;

using Vector3b = Vector<uint8_t, 3>;

using RGBPixel = Vector3b;

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& s, const Vector<T, N>& v)
{
    s << "[";
    std::for_each(std::begin(v),
                  std::end(v) - 1,
                  [&s](const auto c) { s << std::to_string(c) << ", "; });
    return s << std::to_string(v.back()) << "]";
}
}

#endif
