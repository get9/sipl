#pragma once

#ifndef SIPL_IMPROC_IMPROC_H
#define SIPL_IMPROC_IMPROC_H

#include <limits>
#include "matrix/Vector.hpp"
#include "matrix/Matrix.hpp"
#include "improc/NearestNeighborInterpolator.hpp"
#include "improc/BilinearInterpolator.hpp"
#include "improc/Utility.hpp"

namespace sipl
{

enum class InterpolateType { NEAREST_NEIGHBOR, BILINEAR };

// Specialized inverse for mat33d
static Matrix33d inv(const Matrix33d& m)
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

template <typename ElementType, typename InternalType>
MatrixX<ElementType> projective_transform(const MatrixX<ElementType>& image,
                                          const Matrix33d& transform,
                                          const InterpolateType interpolator)
{
    // 1. Figure out sizes of new matrix
    auto c0 = homogenize(transform * Vector3d{0 - 0.5, 0 - 0.5, 1});
    auto c1 = homogenize(transform *
                         Vector3d{0 - 0.5, double(image.dims[0]) - 0.5, 1});
    auto c2 = homogenize(transform *
                         Vector3d{double(image.dims[1]) - 0.5, 0 - 0.5, 1});
    auto c3 = homogenize(transform * Vector3d{double(image.dims[1]) - 0.5,
                                              double(image.dims[0]) - 0.5,
                                              1});
    // Raise or lower values as needed
    for (int32_t i = 0; i < 3; ++i) {
        c0[i] = std::round(c0[i]);
    }
    for (int32_t i = 0; i < 3; ++i) {
        c1[i] = std::round(c1[i]);
    }
    for (int32_t i = 0; i < 3; ++i) {
        c2[i] = std::round(c2[i]);
    }
    for (int32_t i = 0; i < 3; ++i) {
        c3[i] = std::round(c3[i]);
    }

    int32_t ymin = int32_t(std::min({c0[1], c1[1], c2[1], c3[1]}));
    int32_t ymax = int32_t(std::max({c0[1], c1[1], c2[1], c3[1]}));
    int32_t xmin = int32_t(std::min({c0[0], c1[0], c2[0], c3[0]}));
    int32_t xmax = int32_t(std::max({c0[0], c1[0], c2[0], c3[0]}));

    // Create new matrix
    Matrix33d inverse = inv(transform);
    MatrixX<ElementType> new_image(ymax - ymin, xmax - xmin);

    // Start filling it
    for (int32_t i = 0; i < new_image.dims[0]; ++i) {
        for (int32_t j = 0; j < new_image.dims[1]; ++j) {
            Vector3d uv{double(j + xmin), double(i + ymin), double(1)};
            Vector3d xy = homogenize(inverse * uv);
            const double x = xy[0];
            const double y = xy[1];

            // Skip any points outside image space
            if (x < 0 || x >= image.dims[1] || y < 0 || y >= image.dims[0]) {
                new_image(i, j) = ElementType(0);
                continue;
            }

            // Need to interpolate
            switch (interpolator) {
            case InterpolateType::NEAREST_NEIGHBOR: {
                new_image(i, j) =
                    NearestNeighborInterpolator::interpolate<ElementType,
                                                             InternalType>(
                        image, x, y);
                break;
            }
            case InterpolateType::BILINEAR:
                new_image(i, j) =
                    BilinearInterpolator::interpolate<ElementType,
                                                      InternalType>(
                        image, x, y);
                break;
            }
        }
    }

    return new_image;
}

MatrixX<uint8_t> color_to_grayscale(const MatrixX<RgbPixel>& color)
{
    MatrixX<uint8_t> grayscale(color.dims[0], color.dims[1]);
    for (int32_t i = 0; i < color.dims[0]; ++i) {
        for (int32_t j = 0; j < color.dims[1]; ++j) {
            RgbPixel p = color(i, j);
            grayscale(i, j) = clamp(0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]);
        }
    }

    return grayscale;
}
}

#endif
