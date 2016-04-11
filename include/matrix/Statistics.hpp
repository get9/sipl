#pragma once

#ifndef SIPL_MATRIX_STATISTICS_H
#define SIPL_MATRIX_STATISTICS_H

#include "improc/Histogram.hpp"
#include "matrix/Matrix"
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
        std::cout << i << "/" << mats[0].size() << std::endl;

        mat[i] = d;
        hist.fill(0);
    }

    return mat;
}

template <typename Dtype>
MatrixX<Vector3d> average(const std::vector<MatrixX<Dtype>>& mats)
{
    MatrixX<Vector3d> init(mats[0].dims, 0.0);
    auto sum = std::accumulate(std::begin(mats), std::end(mats), init);
    return sum / mats.size();
}
}

#endif
