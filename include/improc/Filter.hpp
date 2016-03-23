#pragma once

#ifndef SIPL_IMPROC_FILTER_H
#define SIPL_IMPROC_FILTER_H

#include <algorithm>
#include <limits>
#include <deque>
#include "Common.hpp"
#include "matrix/Matrix"
#include "matrix/Vector"
#include "improc/Kernels.hpp"
#include "io/BmpIO.hpp"

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

// Thresholds of different types
enum class ThresholdType {
    KEEP_ABOVE,
    KEEP_ABOVE_EQ,
    KEEP_BELOW,
    KEEP_BELOW_EQ,
};

template <typename OutputType, typename InputType>
MatrixX<OutputType> threshold(
    const MatrixX<InputType>& img,
    InputType thresh,
    ThresholdType type,
    OutputType lower = std::numeric_limits<OutputType>::min(),
    OutputType upper = std::numeric_limits<OutputType>::max())
{
    MatrixX<OutputType> result(img.dims, 0);
    switch (type) {
    case ThresholdType::KEEP_ABOVE:
        for (int32_t i = 0; i < img.dims[0]; ++i) {
            for (int32_t j = 0; j < img.dims[1]; ++j) {
                result(i, j) = img(i, j) > thresh ? img(i, j) : lower;
            }
        }
        break;
    case ThresholdType::KEEP_ABOVE_EQ:
        for (int32_t i = 0; i < img.dims[0]; ++i) {
            for (int32_t j = 0; j < img.dims[1]; ++j) {
                result(i, j) = img(i, j) >= thresh ? img(i, j) : lower;
            }
        }
        break;
    case ThresholdType::KEEP_BELOW:
        for (int32_t i = 0; i < img.dims[0]; ++i) {
            for (int32_t j = 0; j < img.dims[1]; ++j) {
                result(i, j) = img(i, j) < thresh ? img(i, j) : upper;
            }
        }
        break;
    case ThresholdType::KEEP_BELOW_EQ:
        for (int32_t i = 0; i < img.dims[0]; ++i) {
            for (int32_t j = 0; j < img.dims[1]; ++j) {
                result(i, j) = img(i, j) <= thresh ? img(i, j) : upper;
            }
        }
        break;
    }

    return result;
}

template <typename Dtype>
MatrixX<Dtype> threshold_binary(const MatrixX<Dtype>& img, Dtype threshold)
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
    auto grad_x = convolve<double>(img, kernels::SobelX);
    auto grad_y = convolve<double>(img, kernels::SobelY);
    return math::hypot(grad_x, grad_y);
}

// Apply Prewitt operator for edge detection
template <typename Dtype>
MatrixX<double> prewitt(const MatrixX<Dtype>& img)
{
    auto grad_x = convolve<double>(img, kernels::PrewittX);
    auto grad_y = convolve<double>(img, kernels::PrewittY);
    return math::hypot(grad_x, grad_y);
}

template <typename Dtype>
MatrixX<Dtype> canny(
    const MatrixX<Dtype>& img, double sigma, double t0, double t1, double t2)
{
    constexpr auto min = std::numeric_limits<Dtype>::min();
    constexpr auto max = std::numeric_limits<Dtype>::max();

    // 1. Smooth with Gaussian filter defined by sigma
    auto smooth = convolve<double>(img, kernels::gaussian_kernel(sigma));

    // 2. Compute gradient (magnitude + direction)
    auto grad_x = convolve<double>(smooth, kernels::SobelX);
    auto grad_y = convolve<double>(smooth, kernels::SobelY);
    auto gradmag = math::hypot(grad_x, grad_y).clip(min, max);
    auto angle = math::atan2(grad_y, grad_x);

    // 3. Initial threshold of the gradient magnitude with threshold t0
    auto mag = threshold<Dtype>(gradmag, t0, ThresholdType::KEEP_ABOVE_EQ);

    // 3. Thin edges using non-maximum suppression
    MatrixXb nonmax(mag.dims, 0);
    for (int32_t i = 1; i < mag.dims[0] - 1; ++i) {
        for (int32_t j = 1; j < mag.dims[1] - 1; ++j) {
            // Skip zero gradient values
            if (mag(i, j) == 0) {
                continue;
            }

            auto a = angle(i, j);

            // Vertical edge
            if ((-M_PI / 8 <= a && a < 0) || (0 <= a && a < M_PI / 8) ||
                (7 * M_PI / 8 <= a && a <= M_PI) ||
                (-M_PI <= a && a < -7 * M_PI / 8)) {
                if (mag(i, j - 1) < mag(i, j) && mag(i, j + 1) < mag(i, j)) {
                    nonmax(i, j) = mag(i, j);
                }
            }

            // Diagonal high to low edge
            else if ((M_PI / 8 <= a && a < 3 * M_PI / 8) ||
                     (-7 * M_PI / 8 <= a && a < -5 * M_PI / 8)) {
                if (mag(i - 1, j - 1) < mag(i, j) &&
                    mag(i + 1, j + 1) < mag(i, j)) {
                    nonmax(i, j) = mag(i, j);
                }
            }

            // Horizontal edge
            else if ((3 * M_PI / 8 <= a && a < 5 * M_PI / 8) ||
                     (-5 * M_PI / 8 <= a && a < -3 * M_PI / 8)) {
                if (mag(i - 1, j) < mag(i, j) && mag(i + 1, j) < mag(i, j)) {
                    nonmax(i, j) = mag(i, j);
                }
            }

            // Diagonal low to high edge
            else if ((5 * M_PI / 8 <= a && a < 7 * M_PI / 8) ||
                     (-3 * M_PI / 8 <= a && a < -M_PI / 8)) {
                if (mag(i - 1, j + 1) < mag(i, j) &&
                    mag(i + 1, j - 1) < mag(i, j)) {
                    nonmax(i, j) = mag(i, j);
                }
            } else {
                std::cout << "shouldn't get here" << std::endl;
            }
        }
    }

    // 4. Link edges
    MatrixXb linked(mag.dims, 0);
    for (int32_t i = 1; i < nonmax.dims[0] - 1; ++i) {
        for (int32_t j = 1; j < nonmax.dims[1] - 1; ++j) {
            // Skip if t2thresh pixel is 0 - it's not an edge
            if (nonmax(i, j) < t2) {
                continue;
            }
            linked(i, j) = max;

            // Use a deque to keep track of pixels we need to track
            // 1. Fill queue with N8 of current pixel in t1thresh if they are an
            // edge
            std::deque<Vector2i> queue;
            for (int32_t m = i - 1; m <= i + 1; ++m) {
                for (int32_t n = j - 1; n <= j + 1; ++n) {
                    if (m == i && n == j) {
                        continue;
                    }
                    if (t1 <= nonmax(m, n) && nonmax(m, n) < t2 &&
                        !linked(m, n)) {
                        queue.push_back({m, n});
                    }
                }
            }

            // 2. Loop until queue is empty, processing each pixel's N8
            while (!queue.empty()) {
                auto p = queue.front();
                queue.pop_front();
                linked(p(0), p(1)) = max;
                for (int32_t m = p(0) - 1; m <= p(0) + 1; ++m) {
                    for (int32_t n = p(1) - 1; n <= p(1) + 1; ++n) {
                        // Bounds checking/skip middle pixel
                        if (m < 0 || m >= nonmax.dims[0] || n < 0 ||
                            n >= nonmax.dims[1] || (m == p(0) && n == p(1))) {
                            continue;
                        }
                        if (t1 <= nonmax(m, n) && nonmax(m, n) < t2 &&
                            !linked(m, n)) {
                            queue.push_back({m, n});
                        }
                    }
                }
            }
        }
    }

    return linked;
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
