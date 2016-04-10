#include "io/PngIO.hpp"
#include "io/PpmIO.hpp"
#include "matrix/Statistics.hpp"
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace sipl;

size_t start_x, start_y, end_x, end_y;
int32_t img_start, img_count;
std::string format;

void parse_commandline(char** argv);

std::vector<std::string> generate_filenames(const std::string& format,
                                            int32_t start,
                                            int32_t count);

MatrixX<RgbPixel> read_png_downsample2(const std::string& filename);

int main(int argc, char** argv)
{
    parse_commandline(argv);
    auto filenames = generate_filenames(format, img_start, img_count);

    std::vector<MatrixX<RgbPixel>> imgs;
    imgs.reserve(filenames.size() / 2);
    for (size_t i = 0; i < filenames.size(); i += 2) {
        imgs.push_back(read_png_downsample2(filenames[i]));
    }

    auto avg_img = average(imgs).clip(0, 255);

    // Compute mode img
    PngIO::write(avg_img.as_type<RgbPixel>(), "mode.png");
}

void parse_commandline(char** argv)
{
    start_x = std::stoul(argv[1]);
    start_y = std::stoul(argv[2]);
    end_x = std::stoul(argv[3]);
    end_y = std::stoul(argv[4]);
    format = std::string(argv[5]);
    img_start = std::stoi(argv[6]);
    img_count = std::stoi(argv[7]);
}

std::vector<std::string> generate_filenames(const std::string& format,
                                            int32_t start,
                                            int32_t count)
{
    std::vector<std::string> filenames;
    for (int32_t i = start; i <= count; ++i) {
        // Code taken from: http://stackoverflow.com/a/26221725
        // + 1 for null terminator
        size_t size = std::snprintf(nullptr, 0, format.c_str(), i) + 1;
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.c_str(), i);
        filenames.emplace_back(buf.get(), buf.get() + size - 1);
    }

    return filenames;
}

MatrixX<RgbPixel> read_png_downsample2(const std::string& filename)
{
    std::vector<uint8_t> pixels;
    uint32_t width, height;
    auto error = lodepng::decode(pixels, width, height, filename,
                                 LodePNGColorType::LCT_RGB, 8);
    if (error) {
        throw std::runtime_error("could not read png");
    }

    MatrixX<RgbPixel> mat(height / 2, width / 2);
    for (size_t i = 0, r = 0; i < height; i += 2, ++r) {
        for (size_t j = 0, c = 0; j < width; j += 2, ++c) {
            mat(r, c)[0] = pixels[i * width * 3 + j * 3 + 0];
            mat(r, c)[1] = pixels[i * width * 3 + j * 3 + 1];
            mat(r, c)[2] = pixels[i * width * 3 + j * 3 + 2];
        }
    }
    return mat;
}
