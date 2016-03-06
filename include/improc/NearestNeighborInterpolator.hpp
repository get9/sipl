#pragma once

#ifndef SIPL_IMPROC_NEARESTNEIGHBORINTERPOLATOR_HPP
#define SIPL_IMPROC_NEARESTNEIGHBORINTERPOLATOR_HPP

#include "matrix/Matrix.hpp"

namespace sipl
{

template <typename InternalType>
struct NearestNeighborInterpolator {
    template <typename Dtype>
    Dtype operator()(const MatrixX<Dtype>& img,
                     double x,
                     double y,
                     Dtype fill_value = Dtype(0))
    {
        auto img_x = int32_t(std::round(x));
        auto img_y = int32_t(std::round(y));
        if (img_x < 0 || img_x >= img.dims[1] || img_y < 0 ||
            img_y >= img.dims[0]) {
            return fill_value;
        }
        return img(img_y, img_x);
    }
};
}

#endif
