#pragma once

#ifndef SIPL_MATRIX_MATRIXOPS_H
#define SIPL_MATRIX_MATRIXOPS_H

namespace sipl
{

template <typename T, int32_t R, int32_t C, typename Scalar>
sipl::Matrix<Scalar, R, C> operator/(const sipl::Matrix<T, R, C>& m, Scalar s)
{
    sipl::Matrix<Scalar, R, C> new_m(m);
    new_m.apply([s](auto e) { return e / s; });
    return new_m;
}

template <typename T, int32_t R, int32_t C, typename Scalar>
sipl::Matrix<Scalar, R, C> operator*(const sipl::Matrix<T, R, C>& m, Scalar s)
{
    sipl::Matrix<Scalar, R, C> new_m(m);
    new_m.apply([s](auto e) { return e * s; });
    return new_m;
}

template <typename T, int32_t R, int32_t C, typename Scalar>
sipl::Matrix<Scalar, R, C> operator*(Scalar s, const sipl::Matrix<T, R, C>& m)
{
    return m * s;
}

template <typename T, int32_t R, int32_t C, typename Scalar>
sipl::Matrix<Scalar, R, C> operator+(const sipl::Matrix<T, R, C>& m, Scalar s)
{
    sipl::Matrix<Scalar, R, C> new_m(m);
    new_m.apply([s](auto e) { return e + s; });
    return new_m;
}

template <typename T, int32_t R, int32_t C, typename Scalar>
sipl::Matrix<Scalar, R, C> operator+(Scalar s, const sipl::Matrix<T, R, C>& m)
{
    return m + s;
}

template <typename T, int32_t R, int32_t C>
sipl::Matrix<T, R, C> operator+(const sipl::Matrix<T, R, C>& m1,
                                const sipl::Matrix<T, R, C>& m2)
{
    sipl::Matrix<T, R, C> new_m(m1.dims);
    for (int32_t i = 0; i < m1.size(); ++i) {
        new_m[i] = m1[i] + m2[i];
    }
    return new_m;
}

template <typename T, int32_t R, int32_t C, typename Scalar>
sipl::Matrix<Scalar, R, C> operator-(const sipl::Matrix<T, R, C>& m, Scalar s)
{
    sipl::Matrix<Scalar, R, C> new_m(m);
    new_m.apply([s](auto e) { return e - s; });
    return new_m;
}

// mat * mat
template <typename T1,
          int32_t R1,
          int32_t C1,
          typename T2,
          int32_t R2,
          int32_t C2>
// No, I can't believe this works either
sipl::Matrix<T1,
             (R1 == Dynamic || C2 == Dynamic ? Dynamic : R1),
             (R1 == Dynamic || C2 == Dynamic ? Dynamic : C2)>
operator*(const sipl::Matrix<T1, R1, C1>& m1,
          const sipl::Matrix<T2, R2, C2>& m2)
{
    assert(m1.dims[1] == m2.dims[0] && "matmul size mismatch");

    sipl::Matrix<T1, (R1 == Dynamic || C2 == Dynamic ? Dynamic : R1),
                 (R1 == Dynamic || C2 == Dynamic ? Dynamic : C2)>
        mat(m1.dims[0], m2.dims[1]);
    for (int32_t row = 0; row < m1.dims[0]; ++row) {
        for (int32_t col = 0; col < m2.dims[1]; ++col) {
            T1 sum = 0;
            for (int32_t inner = 0; inner < m1.dims[1]; ++inner) {
                sum += m1(row, inner) * m2(inner, col);
            }
            mat(row, col) = sum;
        }
    }
    return mat;
}

// mul for mat * vec
template <typename T, int32_t R, int32_t C, typename VT, int32_t Length>
sipl::Vector<T, Length> operator*(const sipl::Matrix<T, R, C>& m,
                                  const sipl::Vector<VT, Length>& v)
{
    assert(m.dims[1] == v.size() && "matmul size mismatch");
    sipl::Vector<T, Length> res(v.size());
    for (int32_t row = 0; row < m.dims[0]; ++row) {
        T sum = 0;
        for (int32_t col = 0; col < m.dims[1]; ++col) {
            sum += m(row, col) * v(col);
        }
        res[row] = sum;
    }
    return res;
}

// Equality
template <typename T1,
          int32_t R1,
          int32_t C1,
          typename T2,
          int32_t R2,
          int32_t C2>
bool operator==(const sipl::Matrix<T1, R1, C1>& lhs,
                const sipl::Matrix<T2, R2, C2>& rhs)
{
    assert(lhs.size() == rhs.size() && "size mismatch");
    for (int32_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i] != rhs[i]) {
            return false;
        }
    }
    return true;
}

template <typename T, int32_t R, int32_t C>
std::ostream& operator<<(std::ostream& s, const sipl::Matrix<T, R, C>& m)
{
    return s << m.str();
}
}

#endif
