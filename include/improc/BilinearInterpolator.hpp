#pragma once

#ifndef SIPL_IMPROC_BILINEARINTERPOLATOR_HPP
#define SIPL_IMPROC_BILINEARINTERPOLATOR_HPP

#include "matrix/Matrix.hpp"
#include "improc/Utility.hpp"

namespace sipl
{

template <typename InternalType>
struct BilinearInterpolator {
    template <typename Dtype>
    Dtype operator()(const MatrixX<Dtype>& img,
                     double x,
                     double y,
                     Dtype fill_value = Dtype(0))
    {
        // Get four pixel values closest to this
        auto x1 = int32_t(std::floor(x));
        auto y1 = int32_t(std::floor(y));
        auto x2 = x1 + 1;
        auto y2 = y1 + 1;

        // Set to fill_value if x1, y1, x2, y2 are outside image range
        if (y1 < 0 || y1 >= img.dims[0] || x1 < 0 || x1 >= img.dims[1] ||
            y2 < 0 || y2 >= img.dims[0] || x2 < 0 || x2 >= img.dims[1]) {
            return fill_value;
        }

        // Do first interpolation (along columns)
        InternalType xy1 = (x2 - x) * img(y1, x1) + (x - x1) * img(y1, x2);
        InternalType xy2 = (x2 - x) * img(y2, x1) + (x - x1) * img(y2, x2);

        // Then linearly interpolate those values
        InternalType f = (y2 - y) * xy1 + (y - y1) * xy2;

        return clamp(f);
    }
};
}

#endif
