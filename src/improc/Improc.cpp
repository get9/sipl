#include "improc/Improc.hpp"

using namespace sipl;

Matrix<uint8_t> color_to_grayscale(const Matrix<RGBPixel>& color)
{
    Matrix<uint8_t> grayscale{color.rows, color.cols};
    for (size_t i = 0; i < color.rows; ++i) {
        for (size_t j = 0; j < color.cols; ++j) {
            uint8_t r, g, b;
            std::tie(r, g, b) = color(i, j);
            grayscale(i, j) = clamp<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
        }
    }
}
