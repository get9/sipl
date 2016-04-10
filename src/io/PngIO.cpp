#include "io/PngIO.hpp"
#include "matrix/Matrix"
#include <algorithm>
#include <vector>

using namespace sipl;

MatrixX<RgbPixel> PngIO::read(const char* filename)
{
    return read(std::string(filename));
}

MatrixX<RgbPixel> PngIO::read(const std::string& filename)
{
    std::vector<uint8_t> raw_pixels;
    uint32_t width, height;
    auto error = lodepng::decode(raw_pixels, width, height, filename,
                                 LodePNGColorType::LCT_RGB, 8);

    if (error) {
        throw IOException("could not load png file");
    }

    MatrixX<RgbPixel> mat(height, width);
    for (size_t i = 0; i < raw_pixels.size() / 3; ++i) {
        RgbPixel pix{raw_pixels[i * 3 + 0], raw_pixels[i * 3 + 1],
                     raw_pixels[i * 3 + 2]};
        mat[i] = pix;
    }
    return mat;
}

void PngIO::write(const MatrixX<RgbPixel>& mat, const char* filename)
{
    write(mat, std::string(filename));
}

void PngIO::write(const MatrixX<RgbPixel>& mat, const std::string& filename)
{
    std::vector<uint8_t> pixels;
    for (int32_t i = 0; i < mat.size(); ++i) {
        pixels.push_back(mat[i][0]);
        pixels.push_back(mat[i][1]);
        pixels.push_back(mat[i][2]);
    }

    auto error = lodepng::encode(filename, pixels, mat.dims[1], mat.dims[0],
                                 LodePNGColorType::LCT_RGB, 8);
    if (error) {
        throw IOException("could not save png");
    }
}
