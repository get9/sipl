#pragma once

#ifndef SIPL_IMPROC_IMPROC_H
#define SIPL_IMPROC_IMPROC_H

#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include "matrix/Vector"
#include "matrix/Matrix"
#include "improc/NearestNeighborInterpolator.hpp"
#include "improc/BilinearInterpolator.hpp"
#include "improc/Utility.hpp"

namespace sipl
{

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

MatrixXb color_to_grayscale(const MatrixX<RgbPixel>& color)
{
    MatrixXb grayscale(color.dims[0], color.dims[1]);
    for (int32_t i = 0; i < color.size(); ++i) {
        RgbPixel p(color[i]);
        grayscale[i] = clamp(0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]);
    }
    return grayscale;
}

// XXX only works for integral-typed matrices for now, need to figure out how to
// get it to work for Vector-type matrices
template <typename Dtype, typename Interpolator>
MatrixX<Dtype> rotate_image(const MatrixX<Dtype>& in_mat,
                            double degrees,
                            const Dtype fill_value = Dtype(0))
{
    const double rads = degrees / 180.0 * M_PI;
    Matrix33d rotation_matrix{{std::cos(rads), std::sin(rads), 0},
                              {-std::sin(rads), std::cos(rads), 0},
                              {0, 0, 1}};
    return projective_transform<Dtype, Interpolator>(in_mat, rotation_matrix,
                                                     fill_value);
}

// Convolution with arbitrary kernel
// XXX Only works with gray-valued images at this time
template <typename Dtype>
MatrixX<Dtype> convolve(const MatrixX<Dtype>& img, const MatrixXd& kernel)
{
    assert(kernel.dims[0] % 2 == 1 && kernel.dims[1] % 2 == 1 &&
           "kernel must have odd # rows and cols");

    MatrixX<Dtype> conv_out(img.dims);
    for (int32_t i = 0; i < img.dims[0]; ++i) {
        for (int32_t j = 0; j < img.dims[1]; ++j) {
            const auto patch =
                img.patch(i, j, kernel.dims[0] / 2, kernel.dims[1] / 2);

            // Do the multiplication of patch and kernel in the correct order
            double sum = 0;
            for (int32_t m_row = 0; m_row < kernel.dims[0]; ++m_row) {
                const auto k_row = kernel.dims[0] - 1 - m_row;
                for (int32_t m_col = 0; m_col < kernel.dims[0]; ++m_col) {
                    const auto k_col = kernel.dims[1] - 1 - m_col;
                    sum += kernel(k_row, k_col) * patch(m_row, m_col);
                }
            }

            // Assign to new matrix position
            conv_out(i, j) = clamp(sum);
        }
    }

    return conv_out;
}

template <typename Dtype>
MatrixX<Dtype> nonlinear_kth_filter(const MatrixX<Dtype>& img,
                                    int32_t height,
                                    int32_t width,
                                    int32_t k)
{
    assert(width % 2 == 1 && height % 2 == 1 && "width and height must be odd");
    assert(k >= 0 && k < width * height && "k out of bounds");

    // For every pixel in the image, get a patch of size height x width around
    // it, sort it, then take the kth element and make that the element we use
    // for the output matrix
    MatrixX<Dtype> result(img.dims);
    for (int32_t i = 0; i < img.dims[0]; ++i) {
        for (int32_t j = 0; j < img.dims[1]; ++j) {
            auto patch = img.patch(i, j, height / 2, width / 2);
            std::sort(patch.data(), patch.data() + patch.size());
            result(i, j) = patch[k];
        }
    }

    return result;
}
}

#endif
