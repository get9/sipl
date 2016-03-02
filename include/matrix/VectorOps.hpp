#include "matrix/Vector.hpp"

// Free-function operations
template <typename Dtype, int32_t Length, typename Scalar>
sipl::Vector<Dtype, Length> operator/(sipl::Vector<Dtype, Length> v, Scalar s)
{
    v /= s;
    return v;
}

template <typename Scalar, typename Dtype, int32_t Length>
sipl::Vector<Dtype, Length> operator*(sipl::Vector<Dtype, Length> v, Scalar s)
{
    v *= s;
    return v;
}

template <typename Scalar, typename Dtype, int32_t Length>
sipl::Vector<Dtype, Length> operator*(Scalar s, sipl::Vector<Dtype, Length> v)
{
    return v * s;
}

template <typename Scalar, typename Dtype, int32_t Length>
sipl::Vector<Dtype, Length> operator+(sipl::Vector<Dtype, Length> v, Scalar s)
{
    v += s;
    return v;
}

template <typename Scalar, typename Dtype, int32_t Length>
sipl::Vector<Dtype, Length> operator+(Scalar s, sipl::Vector<Dtype, Length> v)
{
    return s + v;
}

template <typename Scalar, typename Dtype, int32_t Length>
sipl::Vector<Dtype, Length> operator-(sipl::Vector<Dtype, Length> v, Scalar s)
{
    v -= s;
    return v;
}

// Operators for vectors of different types
template <typename Dtype, typename U, int32_t Length>
sipl::Vector<Dtype, Length> operator+(const sipl::Vector<Dtype, Length>& v1,
                                      const sipl::Vector<U, Length>& v2)
{
    assert(v1.size() == v2.size() && "size mismatch");
    sipl::Vector<Dtype, Length> new_v(v1.size());
    for (int32_t i = 0; i < Length; ++i) {
        new_v[i] = v1[i] + v2[i];
    }
    return new_v;
}

template <typename Dtype, typename U, int32_t Length>
sipl::Vector<Dtype, Length> operator-(const sipl::Vector<Dtype, Length>& v1,
                                      const sipl::Vector<U, Length>& v2)
{
    assert(v1.size() == v2.size() && "size mismatch");
    sipl::Vector<Dtype, Length> new_v(v1.size());
    for (int32_t i = 0; i < Length; ++i) {
        new_v[i] = v1[i] - v2[i];
    }
    return new_v;
}

// Comparison operators
template <typename Dtype, int32_t Length>
bool operator==(const sipl::Vector<Dtype, Length>& lhs,
                const sipl::Vector<Dtype, Length>& rhs)
{
    assert(lhs.size() == rhs.size() && "size mismatch");
    for (int32_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template <typename Dtype, int32_t Length>
bool operator!=(const sipl::Vector<Dtype, Length>& lhs,
                const sipl::Vector<Dtype, Length>& rhs)
{
    return !operator==(lhs, rhs);
}

// XXX Better to compare based on norm or
template <typename Dtype, int32_t Length>
bool operator<(const sipl::Vector<Dtype, Length>& lhs,
               const sipl::Vector<Dtype, Length>& rhs)
{
    return lhs.norm() < rhs.norm();
}

// XXX Better to compare based on norm or
template <typename Dtype, int32_t Length>
bool operator>(const sipl::Vector<Dtype, Length>& lhs,
               const sipl::Vector<Dtype, Length>& rhs)
{
    return operator<(rhs, lhs);
}

// XXX Better to compare based on norm or
template <typename Dtype, int32_t Length>
bool operator<=(const sipl::Vector<Dtype, Length>& lhs,
                const sipl::Vector<Dtype, Length>& rhs)
{
    return !operator>(lhs, rhs);
}

// XXX Better to compare based on norm or
template <typename Dtype, int32_t Length>
bool operator>=(const sipl::Vector<Dtype, Length>& lhs,
                const sipl::Vector<Dtype, Length>& rhs)
{
    return !operator<(lhs, rhs);
}

// Conver to a string
template <typename Dtype, int32_t Length>
std::string as_string(const sipl::Vector<Dtype, Length>& v)
{
    if (v.empty()) {
        return "[]";
    }
    std::string res = "[";
    for (int32_t i = 1; i < v.size() - 1; ++i) {
        res += std::to_string(v[i]) + ", ";
    }
    res += v[v.size() - 1] + "]";
    return res;
}

template <typename Dtype, int32_t Length>
std::ostream& operator<<(std::ostream& s, const sipl::Vector<Dtype, Length>& v)
{
    if (v.size() == 0) {
        return s << "[]";
    }
    s << "[";
    for (int32_t i = 0; i < v.size() - 1; ++i) {
        s << std::to_string(v[i]) << ", ";
    }
    return s << std::to_string(v[v.size() - 1]) << "]";
}
