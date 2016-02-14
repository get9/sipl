#pragma once

#ifndef SIPL_IMPROC_IMPROC_H
#define SIPL_IMPROC_IMPROC_H

#include <limits>
#include "matrix/Vector.hpp"
#include "matrix/Matrix.hpp"

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

uint8_t clamp(const double u)
{
    auto min = std::numeric_limits<uint8_t>::min();
    auto max = std::numeric_limits<uint8_t>::max();
    if (u >= max) {
        return max;
    } else if (u <= min) {
        return min;
    } else {
        return uint8_t(u);
    }
}

RgbPixel clamp(const Vector3d p)
{
    return {clamp(p[0]), clamp(p[1]), clamp(p[2])};
}

template <typename T>
Vector3<T> homogenize(const Vector3<T>& v)
{
    return v / v[2];
}

template <typename T, typename InternalT>
MatrixX<T> projective_transform(const MatrixX<T>& image,
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

    int32_t ymin = std::min({c0[1], c1[1], c2[1], c3[1]});
    int32_t ymax = std::max({c0[1], c1[1], c2[1], c3[1]});
    int32_t xmin = std::min({c0[0], c1[0], c2[0], c3[0]});
    int32_t xmax = std::max({c0[0], c1[0], c2[0], c3[0]});

    // Create new matrix
    Matrix33d inverse = inv(transform);
    MatrixX<T> new_image(ymax - ymin, xmax - xmin);

    // Start filling it
    for (int32_t i = 0; i < new_image.dims[0]; ++i) {
        for (int32_t j = 0; j < new_image.dims[1]; ++j) {
            Vector3d uv{double(j + xmin), double(i + ymin), double(1)};
            Vector3d xy = homogenize(inverse * uv);

            // Skip any points outside image space
            if (xy[0] < 0 || xy[0] >= image.dims[1] || xy[1] < 0 ||
                xy[1] >= image.dims[0]) {
                new_image(i, j) = T(0);
                continue;
            }

            // Need to interpolate
            switch (interpolator) {
            case InterpolateType::NEAREST_NEIGHBOR:
                new_image(i, j) = image(int32_t(std::round(xy[1])),
                                        int32_t(std::round(xy[0])));
                break;
            case InterpolateType::BILINEAR:
                // Get four pixel values closest to this
                double floor_row = std::floor(xy[1]);
                double floor_col = std::floor(xy[0]);
                double ceil_row = std::ceil(xy[1]);
                double ceil_col = std::ceil(xy[0]);

                // Set to 0 if these are outside the range
                if (floor_row < 0 || floor_row >= image.dims[1] ||
                    floor_col < 0 || floor_col >= image.dims[0] ||
                    ceil_row < 0 || ceil_row >= image.dims[1] || ceil_col < 0 ||
                    ceil_col >= image.dims[0]) {
                    new_image(i, j) = T(0);
                }

                // Do first interpolation (along columns)
                InternalT tmp_i1 =
                    ((ceil_col - xy[0]) / (ceil_col - floor_col)) *
                        image(int32_t(floor_row), int32_t(floor_col)) +
                    ((xy[0] - floor_col) / (ceil_col - floor_col)) *
                        image(int32_t(floor_row), int32_t(ceil_col));
                InternalT tmp_i2 =
                    ((ceil_col - xy[0]) / (ceil_col - floor_col)) *
                        image(int32_t(ceil_row), int32_t(floor_col)) +
                    ((xy[0] - floor_col) / (ceil_col - floor_col)) *
                        image(int32_t(ceil_row), int32_t(ceil_col));

                // Then linearly interpolate those values
                InternalT f =
                    ((ceil_row - xy[1]) / (ceil_row - floor_row)) * tmp_i1 +
                    ((xy[1] - floor_row) / (ceil_row - floor_row)) * tmp_i2;

                new_image(i, j) = clamp(f);
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
