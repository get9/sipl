#pragma once

#ifndef SIPL_IMPROC_NEARESTNEIGHBORINTERPOLATOR_HPP
#define SIPL_IMPROC_NEARESTNEIGHBORINTERPOLATOR_HPP

#include "matrix/Matrix.hpp"

namespace sipl
{

struct NearestNeighborInterpolator {
    template <typename Dtype, typename InternalType>
    static Dtype interpolate(const MatrixX<Dtype>& img,
                             const double x,
                             const double y,
                             const Dtype fill_value = Dtype(0))
    {
        const int32_t img_x = int32_t(std::round(x));
        const int32_t img_y = int32_t(std::round(y));
        if (img_x < 0 || img_x >= img.dims[1] || img_y < 0 ||
            img_y >= img.dims[0]) {
            return fill_value;
        }
        return img(img_y, img_x);
    }
};
}

#endif
