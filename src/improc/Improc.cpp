#include "improc/Improc.hpp"

using namespace sipl;

MatrixX<uint8_t> color_to_grayscale(const MatrixX<RgbPixel>& color)
{
    MatrixX<uint8_t> grayscale(color.rows, color.cols);
    for (size_t i = 0; i < color.rows; ++i) {
        for (size_t j = 0; j < color.cols; ++j) {
            RgbPixel p = color(i, j);
            grayscale(i, j) = clamp(0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]);
        }
    }

    return grayscale;
}
