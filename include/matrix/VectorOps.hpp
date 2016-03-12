#pragma once

#ifndef SIPL_MATRIX_VECTOROPS_H
#define SIPL_MATRIX_VECTOROPS_H

#include <iostream>

namespace sipl
{

// Free-function operations
template <typename T, int32_t L, typename Scalar>
sipl::Vector<Scalar, L> operator/(const sipl::Vector<T, L>& v, Scalar s)
{
    sipl::Vector<Scalar, L> new_v(v);
    new_v.apply([s](auto e) { return e / s; });
    return new_v;
}

template <typename T, int32_t L, typename Scalar>
sipl::Vector<Scalar, L> operator*(const sipl::Vector<T, L>& v, Scalar s)
{
    sipl::Vector<Scalar, L> new_v(v);
    new_v.apply([s](auto e) { return e * s; });
    return new_v;
}

template <typename T, int32_t L, typename Scalar>
sipl::Vector<Scalar, L> operator*(Scalar s, const sipl::Vector<T, L>& v)
{
    return v * s;
}

template <typename T, int32_t L, typename Scalar>
sipl::Vector<Scalar, L> operator+(const sipl::Vector<T, L>& v, Scalar s)
{
    sipl::Vector<Scalar, L> new_v(v);
    new_v.apply([s](auto e) { return e + s; });
    return new_v;
}

template <typename T, int32_t L, typename Scalar>
sipl::Vector<Scalar, L> operator+(Scalar s, const sipl::Vector<T, L>& v)
{
    return v + s;
}

template <typename T, int32_t L, typename Scalar>
sipl::Vector<Scalar, L> operator-(sipl::Vector<T, L> v, Scalar s)
{
    sipl::Vector<Scalar, L> new_v(v);
    new_v.apply([s](auto e) { return e - s; });
    return new_v;
}

// Operators for vectors of different types
template <typename T1, int32_t L1, typename T2, int32_t L2>
sipl::Vector<T1, L1> operator+(const sipl::Vector<T1, L1>& v1,
                               const sipl::Vector<T2, L2>& v2)
{
    assert(v1.size() == v2.size() && "size mismatch");
    sipl::Vector<T1, L1> new_v(v1.size());
    for (int32_t i = 0; i < v1.size(); ++i) {
        new_v[i] = v1[i] + v2[i];
    }
    return new_v;
}

// Operators for vectors of different types
template <typename T1, int32_t L1, typename T2, int32_t L2>
sipl::Vector<T1, L1> operator-(const sipl::Vector<T1, L1>& v1,
                               const sipl::Vector<T2, L2>& v2)
{
    assert(v1.size() == v2.size() && "size mismatch");
    sipl::Vector<T1, L1> new_v(v1.size());
    for (int32_t i = 0; i < v1.size(); ++i) {
        new_v[i] = v1[i] - v2[i];
    }
    return new_v;
}

// Comparison operators
template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator==(const sipl::Vector<T1, L1>& lhs,
                const sipl::Vector<T2, L2>& rhs)
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
bool operator!=(const sipl::Vector<T1, L1>& lhs,
                const sipl::Vector<T2, L2>& rhs)
{
    return !operator==(lhs, rhs);
}

// XXX Better to compare based on norm or
template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator<(const sipl::Vector<T1, L1>& lhs, const sipl::Vector<T2, L2>& rhs)
{
    return lhs.norm() < rhs.norm();
}

template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator>(const sipl::Vector<T1, L1>& lhs, const sipl::Vector<T2, L2>& rhs)
{
    return operator<(rhs, lhs);
}

template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator<=(const sipl::Vector<T1, L1>& lhs,
                const sipl::Vector<T2, L2>& rhs)
{
    return !operator>(lhs, rhs);
}

template <typename T1, int32_t L1, typename T2, int32_t L2>
bool operator>=(const sipl::Vector<T1, L1>& lhs,
                const sipl::Vector<T2, L2>& rhs)
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
std::ostream& operator<<(std::ostream& s, const sipl::Vector<Dtype, Length>& v)
{
    return s << v.str();
}
}

#endif
