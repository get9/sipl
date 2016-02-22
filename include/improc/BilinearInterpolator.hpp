#pragma once

#ifndef SIPL_IMPROC_BILINEARINTERPOLATOR_HPP
#define SIPL_IMPROC_BILINEARINTERPOLATOR_HPP

#include "matrix/Matrix.hpp"
#include "improc/Utility.hpp"

namespace sipl
{

struct BilinearInterpolator {
    template <typename Dtype, typename InternalType>
    static Dtype interpolate(const MatrixX<Dtype>& img,
                             const double x,
                             const double y,
                             const Dtype fill_value = Dtype(0))
    {
        // Get four pixel values closest to this
        int32_t floor_row = int32_t(std::floor(y));
        int32_t floor_col = int32_t(std::floor(x));
        int32_t ceil_row = int32_t(std::ceil(y));
        int32_t ceil_col = int32_t(std::ceil(x));

        // Set to 0 if these are outside the range
        if (floor_row < 0 || floor_row >= img.dims[0] || floor_col < 0 ||
            floor_col >= img.dims[1] || ceil_row < 0 ||
            ceil_row >= img.dims[0] || ceil_col < 0 ||
            ceil_col >= img.dims[1]) {
            return fill_value;
        }

        // Do first interpolation (along columns)
        InternalType tmp_i1 = ((ceil_col - x) / (ceil_col - floor_col)) *
                                  img(floor_row, floor_col) +
                              ((x - floor_col) / (ceil_col - floor_col)) *
                                  img(floor_row, ceil_col);
        InternalType tmp_i2 = ((ceil_col - x) / (ceil_col - floor_col)) *
                                  img(ceil_row, floor_col) +
                              ((x - floor_col) / (ceil_col - floor_col)) *
                                  img(ceil_row, ceil_col);

        // Then linearly interpolate those values
        InternalType f = ((ceil_row - y) / (ceil_row - floor_row)) * tmp_i1 +
                         ((y - floor_row) / (ceil_row - floor_row)) * tmp_i2;

        return clamp(f);
    }
};
}

#endif
