#pragma once

#ifndef SIPL_IMPROC_HISTOGRAM_HPP
#define SIPL_IMPROC_HISTOGRAM_HPP

#include <limits>
#include <cstdlib>
#include "matrix/Vector.hpp"
#include "improc/Improc.hpp"

namespace sipl
{

// Calculate histogram of mat
template <typename Dtype>
VectorX<uint32_t> histogram(const MatrixX<Dtype>& mat)
{
    const auto max = std::numeric_limits<Dtype>::max();
    VectorX<uint32_t> hist(max + 1);
    for (int32_t i = 0; i < mat.size(); ++i) {
        hist[mat[i]]++;
    }
    return hist;
}

// Calculate histogram cdf
template <typename Dtype>
VectorX<uint32_t> histogram_cdf(const MatrixX<Dtype>& mat)
{
    const auto hist = histogram(mat);

    double sum = 0;
    VectorX<uint32_t> cdf_hist(hist.size());
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
    for (int32_t i = 0; i < cdf_hist.size(); ++i) {
        if (cdf_hist[i] > 0) {
            cdf_min = cdf_hist[i];
            break;
        }
    }
    constexpr auto max = std::numeric_limits<uint8_t>::max();
    VectorX<uint32_t> equalized_hist(cdf_hist.size());
    for (int32_t i = 0; i < equalized_hist.size(); ++i) {
        if (cdf_hist[i] == 0) {
            equalized_hist[i] = 0;
        } else {
            equalized_hist[i] = std::round(
                ((cdf_hist[i] - cdf_min) / (double(mat.size()) - cdf_min)) *
                max);
        }
    }

    // 2. Compute the new equalized histogram image via lookup
    MatrixX<uint8_t> equalized_hist_img(mat.dims);
    for (int32_t i = 0; i < mat.size(); ++i) {
        equalized_hist_img[i] = equalized_hist[mat[i]];
    }

    return equalized_hist_img;
}

// Convert incoming histogram to an actual image
// For now, only writes out 256x256 histogram image
MatrixX<uint8_t> hist_to_img(const VectorX<uint32_t>& hist)
{
    VectorXd normalized_hist = hist / double(hist.max());
    constexpr int32_t max = std::numeric_limits<uint8_t>::max();
    constexpr int32_t max_size = max + 1;

    // Make a rotated histogram by drawing each row as the number of entries in
    // that bin
    MatrixX<uint8_t> hist_plot(max_size, max_size);
    for (int32_t i = 0; i < hist_plot.dims[0]; ++i) {
        const auto count = int32_t(normalized_hist[i] * max_size);
        for (int32_t j = 0; j < hist_plot.dims[1]; ++j) {
            hist_plot(i, j) = (j < count ? max : 0);
        }
    }

    // Rotate +90 degrees to orient it correctly
    return rotate_image(hist_plot, 90);
}

// Histogram match - return a new matrix (doesn't modify old image)
template <typename Dtype>
MatrixX<Dtype> histogram_match(const MatrixX<Dtype>& target,
                               const MatrixX<Dtype>& source)
{
    // Calculate CDF for both images
    const auto target_cdf = histogram_cdf(target);
    const auto source_cdf = histogram_cdf(source);

    // Build lookup table for matching histograms
    // Note: due to grading requirements of picking the lowest intensity gray
    // level for instances of multimapped values from source CDF to target CDF,
    // iterate backwards so we pick the earlier gray level and use <= min_diff
    constexpr auto max = std::numeric_limits<Dtype>::max();
    VectorX<Dtype> lut(max);
    for (int32_t j = 0; j < lut.size(); ++j) {
        int32_t min_diff = std::numeric_limits<int32_t>::max();
        for (int32_t i = target_cdf.size() - 1; i >= 0; --i) {
            const auto res = std::abs(int32_t(target_cdf[i] - source_cdf[j]));
            if (res <= min_diff) {
                min_diff = res;
                lut[j] = i;
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
}

#endif
