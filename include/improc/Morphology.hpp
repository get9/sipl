#pragma once

#ifndef SIPL_IMPROC_MORPHOLOGY_H
#define SIPL_IMPROC_MORPHOLOGY_H

#include "improc/Filter.hpp"
#include "matrix/Matrix"
#include <limits>

namespace sipl
{

namespace morphology
{

// Create kernels of various types
namespace kernels
{

MatrixXb rectangle(size_t width, size_t height)
{
    assert(width % 2 == 1 && height % 2 == 1 && "width and height must be odd");
    return MatrixXb::Ones(height, width);
}
}

// Note: Assumes img is a binary image
template <typename Dtype>
MatrixX<Dtype> erode(const MatrixX<Dtype>& img, const MatrixXb& element)
{
    assert(element.dims[0] % 2 == 1 && element[1] % 2 == 1 &&
           "element dimensions must be odd");
    MatrixX<Dtype> new_img(img.dims);
    int32_t rheight = element.dims[0] / 2;
    int32_t rwidth = element.dims[1] / 2;
    for (int32_t i = 0; i < img.dims[0]; ++i) {
        for (int32_t j = 0; j < img.dims[1]; ++j) {
            const auto patch = img.patch(i, j, rheight, rwidth);

            // Compute the erosion for this patch
            bool flag = true;
            for (size_t e = 0; e < patch.size(); ++e) {
                if (element[e] && !patch[e]) {
                    flag = false;
                    break;
                }
            }

            // Assign result
            new_img(i, j) = (flag ? std::numeric_limits<Dtype>::max() : 0);
        }
    }

    return new_img;
}

// Note: Assumes img is a binary image
template <typename Dtype>
MatrixX<Dtype> dilate(const MatrixX<Dtype>& img, const MatrixXb& element)
{
    assert(element.dims[0] % 2 == 1 && element[1] % 2 == 1 &&
           "element dimensions must be odd");
    MatrixX<Dtype> new_img(img.dims);
    int32_t rheight = element.dims[0] / 2;
    int32_t rwidth = element.dims[1] / 2;
    for (int32_t i = 0; i < img.dims[0]; ++i) {
        for (int32_t j = 0; j < img.dims[1]; ++j) {
            const auto patch = img.patch(i, j, rheight, rwidth);

            // Compute the erosion for this patch
            bool flag = false;
            for (size_t e = 0; e < patch.size(); ++e) {
                if (element[e] && patch[e]) {
                    flag = true;
                    break;
                }
            }

            // Assign result
            new_img(i, j) = (flag ? std::numeric_limits<Dtype>::max() : 0);
        }
    }

    return new_img;
}

template <typename Dtype>
MatrixX<Dtype> open(const MatrixX<Dtype>& img, const MatrixXb& element)
{
    return dilate(erode(img, element), element);
}

template <typename Dtype>
MatrixX<Dtype> close(const MatrixX<Dtype>& img, const MatrixXb& element)
{
    return erode(dilate(img, element), element);
}
}
}

#endif
