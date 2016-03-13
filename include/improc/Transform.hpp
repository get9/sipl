#pragma once

#ifndef SIPL_IMPROC_TRANSFORM_H
#define SIPL_IMPROC_TRANSFORM_H

#include "matrix/Matrix"
#include "matrix/Vector"
#include "improc/BilinearInterpolator.hpp"
#include "improc/NearestNeighborInterpolator.hpp"
#include "Common.hpp"

namespace sipl
{

// Possible interpolation types
enum class InterpolateType { BILINEAR, NEAREST_NEIGHBOR, UNKNOWN };

// Specialized inverse for mat33d
inline static Matrix33d inv(const Matrix33d& m)
{
    double determinant = m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) -
                         m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
                         m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
    Matrix33d inverse = {{m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1),
                          m(0, 2) * m(2, 1) - m(0, 1) * m(2, 2),
                          m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)},
                         {m(1, 2) * m(2, 0) - m(1, 0) * m(2, 2),
                          m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0),
                          m(0, 2) * m(1, 0) - m(0, 0) * m(1, 2)},
                         {m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0),
                          m(0, 1) * m(2, 0) - m(0, 0) * m(2, 1),
                          m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0)}};
    return inverse / determinant;
}

template <typename Interpolator, typename ElementType>
MatrixX<ElementType> projective_transform(
    const MatrixX<ElementType>& image,
    const Matrix33d& transform,
    const ElementType fill_value = ElementType(0))
{
    // 1. Figure out sizes of new matrix
    auto c0 = homogenize(transform * Vector3d{-0.5, -0.5, 1});
    auto c1 = homogenize(transform * Vector3d{-0.5, image.dims[0] - 0.5, 1});
    auto c2 = homogenize(transform * Vector3d{image.dims[1] - 0.5, -0.5, 1});
    auto c3 = homogenize(transform *
                         Vector3d{image.dims[1] - 0.5, image.dims[0] - 0.5, 1});

    // Raise or lower values as needed
    auto xs = Vector4d{c0[0], c1[0], c2[0], c3[0]};
    auto ys = Vector4d{c0[1], c1[1], c2[1], c3[1]};

    // Create new matrix
    Matrix33d inverse = inv(transform);
    MatrixX<ElementType> new_image(int32_t(ys.max() - ys.min()),
                                   int32_t(xs.max() - xs.min()));

    // Do interpolation for each output pixel
    Interpolator interp;
    for (int32_t i = 0; i < new_image.dims[0]; ++i) {
        for (int32_t j = 0; j < new_image.dims[1]; ++j) {
            Vector3d uv{j + xs.min(), i + ys.min(), 1};
            Vector3d xy = homogenize(inverse * uv);
            new_image(i, j) = interp(image, xy[0], xy[1], fill_value);
        }
    }

    return new_image;
}

// XXX only works for integral-typed matrices for now, need to figure out how to
// get it to work for Vector-type matrices
template <typename Dtype, typename Interpolator>
MatrixX<Dtype> rotate_image(const MatrixX<Dtype>& in_mat,
                            double degrees,
                            const Dtype fill_value = Dtype(0))
{
    auto rads = deg2rad(degrees);
    Matrix33d rotation_matrix{{std::cos(rads), std::sin(rads), 0},
                              {-std::sin(rads), std::cos(rads), 0},
                              {0, 0, 1}};
    return projective_transform<Interpolator>(in_mat, rotation_matrix,
                                              fill_value);
}
}

#endif
