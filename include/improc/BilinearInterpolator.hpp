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
        // Note: need to do this hackery in cases where x and y are exact
        // integers
        double d_col0, d_row0;
        double xfrac = int32_t(std::modf(x, &d_col0));
        double yfrac = int32_t(std::modf(y, &d_row0));
        int32_t col0 = int32_t(d_col0);
        int32_t row0 = int32_t(d_row0);
        int32_t col1 = (xfrac == 0 ? col0 + 1 : int32_t(std::ceil(x)));
        int32_t row1 = (yfrac == 0 ? row0 + 1 : int32_t(std::ceil(y)));

        // Set to 0 if these are outside the range
        if (row0 < 0 || row0 >= img.dims[0] || col0 < 0 ||
            col0 >= img.dims[1] || row1 < 0 || row1 >= img.dims[0] ||
            col1 < 0 || col1 >= img.dims[1]) {
            return fill_value;
        }

        // Do first interpolation (along columns)
        InternalType tmp_i1 = ((col1 - x) / (col1 - col0)) * img(row0, col0) +
                              ((x - col0) / (col1 - col0)) * img(row0, col1);
        InternalType tmp_i2 = ((col1 - x) / (col1 - col0)) * img(row1, col0) +
                              ((x - col0) / (col1 - col0)) * img(row1, col1);

        // Then linearly interpolate those values
        InternalType f = ((row1 - y) / (row1 - row0)) * tmp_i1 +
                         ((y - row0) / (row1 - row0)) * tmp_i2;

        std::cout << f << std::endl;
        return clamp(f);
    }
};
}

#endif
