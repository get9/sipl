#pragma once

#ifndef SIPL_IMPROC_KERNELS_H
#define SIPL_IMPROC_KERNELS_H

#include "matrix/Matrix"

namespace sipl
{

namespace kernels
{
// clang-format off
static const Matrix33d SobelX{{-1, 0, 1},
                              {-2, 0, 2},
                              {-1, 0, 1}};

static const Matrix33d SobelY{{-1, -2, -1},
                              { 0,  0,  0},
                              { 1,  2,  1}};

static const Matrix33d PrewittX{{-1, 0, 1},
                                {-1, 0, 1},
                                {-1, 0, 1}};

static const Matrix33d PrewittY{{-1, -1, -1},
                                { 0,  0,  0},
                                { 1,  1,  1}};

static const Matrix33d ScharrX{{ -3, 0,  3},
                               {-10, 0, 10},
                               { -3, 0,  3}};

static const Matrix33d ScharrY{{-3, -10, -3},
                               { 0,   0,  0},
                               { 3,  10,  3}};
// clang-format on

// Create a Gaussian kernel with size sigma
MatrixX<double> gaussian_kernel(double sigma, double scale = 0)
{
    // Calculation of size comes from Homework 4
    auto size = int32_t(std::round(2 * std::abs(2 * sigma) + 1));
    MatrixX<double> kernel(size, size);

    // Cache coefficients
    double coeff = 1.0 / std::sqrt(2 * M_PI * sigma * sigma);
    double denom = 2 * sigma * sigma;

    // Iterates from -n ... +n where n is size / 2 (using integer division)
    for (int32_t i = -size / 2, a = 0; i <= size / 2; ++i, ++a) {
        for (int32_t j = -size / 2, b = 0; j <= size / 2; ++j, ++b) {
            kernel(a, b) = coeff * std::exp(-(i * i + j * j) / denom);
        }
    }

    // Optional scaling
    if (scale != 0) {
        kernel *= scale;
        kernel.transform([](auto e) { return std::round(e); });
    }

    return kernel / kernel.sum();
}
}
}

#endif
