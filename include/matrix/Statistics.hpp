#pragma once

#ifndef SIPL_MATRIX_STATISTICS_H
#define SIPL_MATRIX_STATISTICS_H

#include "improc/Histogram.hpp"
#include "matrix/Matrix"
#include <algorithm>
#include <vector>

namespace sipl
{

template <typename Dtype>
MatrixX<Dtype> mode(const std::vector<MatrixX<Dtype>>& mats)
{
    MatrixX<Dtype> mat(mats[0].dims);
    Dtype d;
    Vector<size_t, 256> hist(size_t(0));
    for (int32_t i = 0; i < mats[0].size(); ++i) {
        for (int32_t c = 0; c < 2; ++c) {

            // Calculate histogram
            for (auto m : mats) {
                hist[m[i][c]]++;
            }
            d[c] = hist.argmax();
        }

        mat[i] = d;
        hist.fill(0);
    }

    return mat;
}

template <typename Dtype>
MatrixXd average(const std::vector<MatrixX<Dtype>>& mats)
{
    MatrixXd init(mats[0].dims, 0.0);
    auto sum = std::accumulate(std::begin(mats), std::end(mats), init);
    return sum / double(mats.size());
}

template <typename Dtype>
MatrixX<Dtype> median(const std::vector<MatrixX<Dtype>>& mats)
{
    auto dims = mats[0].dims;
    MatrixX<Dtype> med(dims[0] / 2, dims[1] / 2);
    for (int32_t i = 0, a = 0; i < dims[0]; i += 2, ++a) {
        for (int32_t j = 0, b = 0; j < dims[1]; j += 2, ++b) {
            std::vector<Dtype> pixels(mats.size());
            std::transform(std::begin(mats), std::end(mats), std::begin(pixels),
                           [i, j](auto e) { return e(i, j); });
            std::nth_element(std::begin(pixels),
                             std::begin(pixels) + pixels.size() / 2,
                             std::end(pixels));
            med(a, b) = pixels[pixels.size() / 2];
        }
    }

    return med;
}
}

#endif
