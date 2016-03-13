#pragma once

#ifndef SIPL_IMPROC_FILTER_H
#define SIPL_IMPROC_FILTER_H

#include <algorithm>
#include <limits>
#include "Common.hpp"
#include "matrix/Matrix"
#include "matrix/Vector"
#include "improc/Kernels.hpp"

namespace sipl
{

// Convolution with arbitrary kernel
// XXX Only works with gray-valued images at this time
template <typename OutputType, typename InputType, int32_t Rows, int32_t Cols>
MatrixX<OutputType> convolve(const MatrixX<InputType>& img,
                             const Matrix<double, Rows, Cols>& kernel)
{
    assert(kernel.dims[0] % 2 == 1 && kernel.dims[1] % 2 == 1 &&
           "kernel must have odd # rows and cols");

    MatrixX<OutputType> conv(img.dims);
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
            conv(i, j) = clamp<OutputType>(sum);
        }
    }

    return conv;
}

template <typename OutputType, typename InputType, int32_t Rows, int32_t Cols>
MatrixX<OutputType> correlate(const MatrixX<InputType>& img,
                              const Matrix<double, Rows, Cols>& kernel)
{
    assert(kernel.dims[0] % 2 == 1 && kernel.dims[1] % 2 == 1 &&
           "kernel must have odd # rows and cols");

    MatrixX<OutputType> corr(img.dims);
    for (int32_t i = 0; i < img.dims[0]; ++i) {
        for (int32_t j = 0; j < img.dims[1]; ++j) {
            const auto patch =
                img.patch(i, j, kernel.dims[0] / 2, kernel.dims[1] / 2);

            // Correlation is element-by-element multiplication
            double sum = 0;
            for (int32_t k = 0; k < patch.size(); ++k) {
                sum += patch[k] * kernel[k];
            }

            // Assign to new matrix position
            corr(i, j) = clamp<OutputType>(sum);
        }
    }

    return corr;
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
            std::sort(std::begin(patch), std::end(patch));
            result(i, j) = patch[k];
        }
    }

    return result;
}

// Threshold in-place
template <typename Dtype>
MatrixX<Dtype> threshold_binary(const MatrixX<Dtype>& img, int32_t threshold)
{
    const auto min = std::numeric_limits<Dtype>::min();
    const auto max = std::numeric_limits<Dtype>::max();

    if (threshold < min || threshold > max) {
        throw std::invalid_argument(
            "threshold must be between min and max for Dtype");
    }

    MatrixX<Dtype> thresh(img.dims);
    for (int32_t i = 0; i < img.size(); ++i) {
        thresh[i] = (img[i] >= threshold ? max : min);
    }
    return thresh;
}

// Apply Sobel operator for edge detection
template <typename Dtype>
MatrixX<double> sobel(const MatrixX<Dtype>& img)
{
    auto grad_x = math::square(correlate<double>(img, Kernels::SobelX));
    auto grad_y = math::square(correlate<double>(img, Kernels::SobelY));
    return math::sqrt(grad_x + grad_y);
}

// Apply Prewitt operator for edge detection
template <typename Dtype>
MatrixX<double> prewitt(const MatrixX<Dtype>& img)
{
    auto grad_x = math::square(correlate<double>(img, Kernels::PrewittX));
    auto grad_y = math::square(correlate<double>(img, Kernels::PrewittY));
    return math::sqrt(grad_x + grad_y);
}

// Convert a color image to grayscale
MatrixXb color_to_grayscale(const MatrixX<RgbPixel>& color)
{
    MatrixXb grayscale(color.dims[0], color.dims[1]);
    for (int32_t i = 0; i < color.size(); ++i) {
        RgbPixel p(color[i]);
        grayscale[i] =
            clamp<uint8_t>(0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]);
    }
    return grayscale;
}
}

#endif
