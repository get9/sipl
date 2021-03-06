#pragma once

#ifndef SIPL_IMPROC_HISTOGRAM_HPP
#define SIPL_IMPROC_HISTOGRAM_HPP

#include <limits>
#include <cstdlib>
#include "Common.hpp"
#include "matrix/Matrix"
#include "matrix/Vector"
#include "improc/Transform.hpp"

namespace sipl
{

// Calculate histogram of mat
template <typename Dtype>
VectorX<uint32_t> histogram(const MatrixX<Dtype>& mat)
{
    const int32_t max = std::numeric_limits<Dtype>::max();
    VectorX<uint32_t> hist(max + 1, 0);
    for (const auto e : mat) {
        hist[e]++;
    }
    return hist;
}

// Calculate histogram cdf
template <typename Dtype>
VectorX<uint32_t> histogram_cdf(const MatrixX<Dtype>& mat)
{
    const auto hist = histogram(mat);

    uint32_t sum = 0;
    VectorX<uint32_t> cdf_hist(hist.size(), 0);
    for (int32_t i = 0; i < cdf_hist.size(); ++i) {
        sum += hist[i];
        cdf_hist[i] = sum;
    }

    return cdf_hist;
}

template <typename Dtype>
MatrixX<Dtype> equalize_hist(const MatrixX<Dtype>& mat)
{
    // 1. Compute the equalized histogram
    const auto cdf_hist = histogram_cdf(mat);

    // Find first nonzero cdf hist value that will be cdf_min
    uint32_t cdf_min = 0;
    for (const auto bin : cdf_hist) {
        if (bin > 0) {
            cdf_min = bin;
            break;
        }
    }

    constexpr int32_t max = std::numeric_limits<uint8_t>::max();
    VectorX<uint32_t> equalized_hist(cdf_hist.size());
    for (int32_t i = 0; i < equalized_hist.size(); ++i) {
        if (cdf_hist[i] == 0) {
            equalized_hist[i] = 0;
        } else {
            equalized_hist[i] = uint32_t(std::nearbyint(
                ((cdf_hist[i] - cdf_min) / (double(mat.size()) - cdf_min)) *
                max));
        }
    }

    // 2. Compute the new equalized histogram image via lookup
    MatrixX<uint8_t> equalized_hist_img(mat.dims);
    for (int32_t i = 0; i < mat.size(); ++i) {
        equalized_hist_img[i] = uint8_t(equalized_hist[mat[i]]);
    }

    return equalized_hist_img;
}

// Histogram match - return a new matrix (doesn't modify old image)
template <typename Dtype>
MatrixX<Dtype> histogram_match(const MatrixX<Dtype>& target,
                               const MatrixX<Dtype>& source)
{
    // Calculate CDF for both images
    const auto target_cdf = histogram_cdf(target);
    const auto source_cdf = histogram_cdf(source);
    const auto target_cdf_norm =
        target_cdf / double(target_cdf[target_cdf.size() - 1]);
    const auto source_cdf_norm =
        source_cdf / double(source_cdf[source_cdf.size() - 1]);

    // Build lookup table for matching histograms
    // Note: due to grading requirements of picking the lowest intensity gray
    // level for instances of multimapped values from source CDF to target CDF,
    // iterate backwards so we pick the earlier gray level and use <= min_diff
    constexpr int32_t max = int32_t(std::numeric_limits<uint8_t>::max() + 1);
    VectorX<Dtype> lut(max, 0);
    for (int32_t j = 0; j < lut.size(); ++j) {
        int32_t min_diff = std::numeric_limits<int32_t>::max();
        for (int32_t i = target_cdf.size() - 1; i >= 0; --i) {
            const int32_t res = std::abs(int32_t(
                std::round((target_cdf_norm[i] - source_cdf_norm[j]) * max)));
            if (res <= min_diff) {
                min_diff = res;
                lut[j] = Dtype(i);
            }
        }
    }

    // Alter the histogram of the source image to match target image via the LUT
    MatrixX<Dtype> modified_source(source.dims);
    for (int32_t i = 0; i < source.dims[0]; ++i) {
        for (int32_t j = 0; j < source.dims[1]; ++j) {
            modified_source(i, j) = lut[source(i, j)];
        }
    }

    return modified_source;
}

// Convert incoming histogram to an actual image
// For now, only writes out 256x256 histogram image
MatrixX<uint8_t> hist_to_img(const VectorX<uint32_t>& hist)
{
    constexpr auto max = int32_t(std::numeric_limits<uint8_t>::max());
    constexpr int32_t max_size = max + 1;

    // Make a rotated histogram by drawing each row as the number of entries in
    // that bin
    MatrixXb hist_plot(max_size, max_size);
    for (int32_t i = 0; i < hist_plot.dims[0]; ++i) {
        int32_t count =
            int32_t(std::round(hist[i] / double(hist.max()) * max_size));
        for (int32_t j = 0; j < hist_plot.dims[1]; ++j) {
            hist_plot(i, j) = (j < count ? 0 : max);
        }
    }

    // Rotate +90 degrees
    using Interpolator = BilinearInterpolator<double>;
    return rotate_image<uint8_t, Interpolator>(hist_plot, 90, max);
}
}

#endif
