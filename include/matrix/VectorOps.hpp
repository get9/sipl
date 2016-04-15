#pragma once

#ifndef SIPL_MATRIX_VECTOROPS_H
#define SIPL_MATRIX_VECTOROPS_H

#include "matrix/Vector"
#include <iostream>

namespace sipl
{

// Free-function operations
template <typename T,
          int32_t Length,
          typename Scalar,
          typename = typename std::enable_if<std::is_arithmetic<Scalar>::value,
                                             Scalar>::type>
auto operator/(const Vector<T, Length>& v, Scalar s)
    -> Vector<decltype(v.front() / s), Length>
{
    return v.apply([s](auto e) { return e / s; });
}

template <typename T,
          int32_t Length,
          typename Scalar,
          typename = typename std::enable_if<std::is_arithmetic<Scalar>::value,
                                             Scalar>::type>
auto operator*(const Vector<T, Length>& v, Scalar s)
    -> Vector<decltype(v.front() * s), Length>
{
    return v.apply([s](auto e) { return e * s; });
}

template <typename T,
          int32_t Length,
          typename Scalar,
          typename = typename std::enable_if<std::is_arithmetic<Scalar>::value,
                                             Scalar>::type>
auto operator*(Scalar s, const Vector<T, Length>& v)
    -> Vector<decltype(v.front() * s), Length>
{
    return v * s;
}

template <typename T,
          int32_t Length,
          typename Scalar,
          typename = typename std::enable_if<std::is_arithmetic<Scalar>::value,
                                             Scalar>::type>
auto operator+(const Vector<T, Length>& v, Scalar s)
    -> Vector<decltype(v.front() + s), Length>
{
    return v.apply([s](auto e) { return e + s; });
}

template <typename T,
          int32_t Length,
          typename Scalar,
          typename = typename std::enable_if<std::is_arithmetic<Scalar>::value,
                                             Scalar>::type>
auto operator+(Scalar s, const Vector<T, Length>& v)
    -> Vector<decltype(v.front() + s), Length>
{
    return v + s;
}

template <typename T,
          int32_t Length,
          typename Scalar,
          typename = typename std::enable_if<std::is_arithmetic<Scalar>::value,
                                             Scalar>::type>
auto operator-(const Vector<T, Length>& v, Scalar s)
    -> Vector<decltype(v.front() - s), Length>
{
    return v.apply([s](auto e) { return e - s; });
}

// Operators for vectors of different types
template <typename T1,
          int32_t L1,
          typename T2,
          int32_t L2,
          int32_t ResultLength = L1 == Dynamic || L2 == Dynamic ? Dynamic : L1>
auto operator+(const Vector<T1, L1>& v1, const Vector<T2, L2>& v2)
    -> Vector<decltype(v1.front() + v2.front()), ResultLength>
{
    assert(v1.size() == v2.size() && "size mismatch");
    Vector<T1, ResultLength> new_v(v1.size());
    for (int32_t i = 0; i < v1.size(); ++i) {
        new_v[i] = v1[i] + v2[i];
    }
    return new_v;
}

// Operators for vectors of different types
template <typename T1,
          int32_t L1,
          typename T2,
          int32_t L2,
          int32_t ResultLength = L1 == Dynamic || L2 == Dynamic ? Dynamic : L1>
auto operator-(const Vector<T1, L1>& v1, const Vector<T2, L2>& v2)
    -> Vector<decltype(v1.front() - v2.front()), ResultLength>
{
    assert(v1.size() == v2.size() && "size mismatch");
    using ResultType = decltype(v1.front() - v2.front());
    Vector<ResultType, ResultLength> new_v(v1.size());
    for (int32_t i = 0; i < v1.size(); ++i) {
        new_v[i] = v1[i] - v2[i];
    }
    return new_v;
}

// Comparison operators
template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator==(const Vector<T1, L1>& lhs, const Vector<T2, L2>& rhs)
{
    assert(lhs.size() == rhs.size() && "size mismatch");
    for (int32_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator!=(const Vector<T1, L1>& lhs, const Vector<T2, L2>& rhs)
{
    return !operator==(lhs, rhs);
}

// XXX Better to compare based on norm or
template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator<(const Vector<T1, L1>& lhs, const Vector<T2, L2>& rhs)
{
    return lhs.norm() < rhs.norm();
}

template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator>(const Vector<T1, L1>& lhs, const Vector<T2, L2>& rhs)
{
    return operator<(rhs, lhs);
}

template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator<=(const Vector<T1, L1>& lhs, const Vector<T2, L2>& rhs)
{
    return !operator>(lhs, rhs);
}

template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator>=(const Vector<T1, L1>& lhs, const Vector<T2, L2>& rhs)
{
    return !operator<(lhs, rhs);
}

// Make a vector homogenous
template <typename T, int32_t Length>
Vector<double, Length> homogenize(const Vector<T, Length>& v)
{
    return v / double(v.back());
}

// Convert to a string
// XXX Need to properly constrain this template so it's not a catch-all for
// other operator<<'s.
template <typename Dtype, int32_t Length>
std::ostream& operator<<(std::ostream& s, const Vector<Dtype, Length>& v)
{
    return s << v.str();
}

// Other generic operations on each individual element
// Put in their own namespace so they don't collide with STL functions
// Use decltype(auto) for return type --> let Matrix::apply handle deducing
// operation return type
namespace math
{

template <typename Dtype, int32_t Length>
decltype(auto) square(const Vector<Dtype, Length>& v)
{
    return v.apply([](auto e) { return e * e; });
}

template <typename Dtype, int32_t Length>
decltype(auto) sqrt(const Vector<Dtype, Length>& v)
{
    return v.apply([](auto e) { return std::sqrt(e); });
}

template <typename Dtype, int32_t Length>
decltype(auto) pow(const Vector<Dtype, Length>& m, double exp)
{
    return m.apply([exp](auto e) { return std::pow(e, exp); });
}
}
}

#endif
