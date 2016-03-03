#include "matrix/VectorBase.hpp"

// Free-function operations
template <typename T, int32_t L, typename C, typename Scalar>
sipl::VectorBase<T, L, C> operator/(
    sipl::VectorBase<T, L, C> v,
    typename std::enable_if<std::is_arithmetic<Scalar>::value>::type s)
{
    v /= s;
    return v;
}

template <typename T, int32_t L, typename C, typename Scalar>
sipl::VectorBase<T, L, C> operator*(
    sipl::VectorBase<T, L, C> v,
    typename std::enable_if<std::is_arithmetic<Scalar>::value>::type s)
{
    v *= s;
    return v;
}

template <typename T, int32_t L, typename C, typename Scalar>
sipl::VectorBase<T, L, C> operator*(
    typename std::enable_if<std::is_arithmetic<Scalar>::value>::type s,
    sipl::VectorBase<T, L, C> v)
{
    return v * s;
}

template <typename T, int32_t L, typename C, typename Scalar>
sipl::VectorBase<T, L, C> operator+(
    sipl::VectorBase<T, L, C> v,
    typename std::enable_if<std::is_arithmetic<Scalar>::value>::type s)
{
    v += s;
    return v;
}

template <typename T, int32_t L, typename C, typename Scalar>
sipl::VectorBase<T, L, C> operator+(
    typename std::enable_if<std::is_arithmetic<Scalar>::value>::type s,
    sipl::VectorBase<T, L, C> v)
{
    return s + v;
}

template <typename T, int32_t L, typename C, typename Scalar>
sipl::VectorBase<T, L, C> operator-(
    sipl::VectorBase<T, L, C> v,
    typename std::enable_if<std::is_arithmetic<Scalar>::value>::type s)
{
    v -= s;
    return v;
}

// Operators for vectors of different types
template <typename T1,
          int32_t L1,
          typename C1,
          typename T2,
          int32_t L2,
          typename C2>
sipl::VectorBase<T1, L1, C1> operator+(const sipl::VectorBase<T1, L1, C1>& v1,
                                       const sipl::VectorBase<T2, L2, C2>& v2)
{
    assert(v1.size() == v2.size() && "size mismatch");
    sipl::VectorBase<T1, L1, C1> new_v(v1.size());
    for (int32_t i = 0; i < v1.size(); ++i) {
        new_v[i] = v1[i] + v2[i];
    }
    return new_v;
}

template <typename T1,
          int32_t L1,
          typename C1,
          typename T2,
          int32_t L2,
          typename C2>
sipl::VectorBase<T1, L1, C1> operator-(const sipl::VectorBase<T1, L1, C1>& v1,
                                       const sipl::VectorBase<T2, L2, C2>& v2)
{
    assert(v1.size() == v2.size() && "size mismatch");
    sipl::VectorBase<T1, L1, C1> new_v(v1.size());
    for (int32_t i = 0; i < v1.size(); ++i) {
        new_v[i] = v1[i] - v2[i];
    }
    return new_v;
}

// Comparison operators
template <typename T1,
          int32_t L1,
          typename C1,
          typename T2,
          int32_t L2,
          typename C2>
bool operator==(const sipl::VectorBase<T1, L1, C1>& lhs,
                const sipl::VectorBase<T2, L2, C2>& rhs)
{
    assert(lhs.size() == rhs.size() && "size mismatch");
    for (int32_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template <typename T1,
          int32_t L1,
          typename C1,
          typename T2,
          int32_t L2,
          typename C2>
bool operator!=(const sipl::VectorBase<T1, L1, C1>& lhs,
                const sipl::VectorBase<T2, L2, C2>& rhs)
{
    return !operator==(lhs, rhs);
}

// XXX Better to compare based on norm or
template <typename T1,
          int32_t L1,
          typename C1,
          typename T2,
          int32_t L2,
          typename C2>
bool operator<(const sipl::VectorBase<T1, L1, C1>& lhs,
               const sipl::VectorBase<T2, L2, C2>& rhs)
{
    return lhs.norm() < rhs.norm();
}

template <typename T1,
          int32_t L1,
          typename C1,
          typename T2,
          int32_t L2,
          typename C2>
bool operator>(const sipl::VectorBase<T1, L1, C1>& lhs,
               const sipl::VectorBase<T2, L2, C2>& rhs)
{
    return operator<(rhs, lhs);
}

template <typename T1,
          int32_t L1,
          typename C1,
          typename T2,
          int32_t L2,
          typename C2>
bool operator<=(const sipl::VectorBase<T1, L1, C1>& lhs,
                const sipl::VectorBase<T2, L2, C2>& rhs)
{
    return !operator>(lhs, rhs);
}

template <typename T1,
          int32_t L1,
          typename C1,
          typename T2,
          int32_t L2,
          typename C2>
bool operator>=(const sipl::VectorBase<T1, L1, C1>& lhs,
                const sipl::VectorBase<T2, L2, C2>& rhs)
{
    return !operator<(lhs, rhs);
}

// Convert to a string
// XXX Need to properly constrain this template so it's not a catch-all for
// other operator<<'s.
template <typename Dtype, int32_t Length, typename Container>
std::ostream& operator<<(std::ostream& s,
                         const sipl::VectorBase<Dtype, Length, Container>& v)
{
    return s << v.str();
}
