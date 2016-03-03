#include "matrix/VectorBase.hpp"

// Free-function operations
template <typename VectorType, typename Scalar>
VectorType operator/(
    VectorType v,
    typename std::enable_if<std::is_integral<Scalar>::value>::type s)
{
    v /= s;
    return v;
}

template <typename VectorType, typename Scalar>
VectorType operator*(VectorType v, Scalar s)
{
    v *= s;
    return v;
}

template <typename VectorType, typename Scalar>
VectorType operator*(Scalar s, VectorType v)
{
    return v * s;
}

template <typename VectorType, typename Scalar>
VectorType operator+(
    VectorType v,
    typename std::enable_if<std::is_integral<Scalar>::value>::type s)
{
    v += s;
    return v;
}

template <typename VectorType, typename Scalar>
VectorType operator+(
    typename std::enable_if<std::is_integral<Scalar>::value>::type s,
    VectorType v)
{
    return s + v;
}

template <typename VectorType, typename Scalar>
VectorType operator-(VectorType v, Scalar s)
{
    v -= s;
    return v;
}

// Operators for vectors of different types
template <typename VectorType1, typename VectorType2>
VectorType1 operator+(const VectorType1& v1, const VectorType2& v2)
{
    assert(v1.size() == v2.size() && "size mismatch");
    VectorType1 new_v(v1.size());
    for (int32_t i = 0; i < v1.size(); ++i) {
        new_v[i] = v1[i] + v2[i];
    }
    return new_v;
}

template <typename VectorType1, typename VectorType2>
VectorType1 operator-(const VectorType1& v1, const VectorType2& v2)
{
    assert(v1.size() == v2.size() && "size mismatch");
    VectorType1 new_v(v1.size());
    for (int32_t i = 0; i < v1.size(); ++i) {
        new_v[i] = v1[i] - v2[i];
    }
    return new_v;
}

// Comparison operators
template <typename VectorType1, typename VectorType2>
bool operator==(const VectorType1& lhs, const VectorType2& rhs)
{
    assert(lhs.size() == rhs.size() && "size mismatch");
    for (int32_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template <typename VectorType1, typename VectorType2>
bool operator!=(const VectorType1& lhs, const VectorType2& rhs)
{
    return !operator==(lhs, rhs);
}

// XXX Better to compare based on norm or
template <typename VectorType1, typename VectorType2>
bool operator<(const VectorType1& lhs, const VectorType2& rhs)
{
    return lhs.norm() < rhs.norm();
}

template <typename VectorType1, typename VectorType2>
bool operator>(const VectorType1& lhs, const VectorType2& rhs)
{
    return operator<(rhs, lhs);
}

template <typename VectorType1, typename VectorType2>
bool operator<=(const VectorType1& lhs, const VectorType2& rhs)
{
    return !operator>(lhs, rhs);
}

template <typename VectorType1, typename VectorType2>
bool operator>=(const VectorType1& lhs, const VectorType2& rhs)
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
