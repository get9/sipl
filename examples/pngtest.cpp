#include "improc/Filter.hpp"
#include "improc/Morphology.hpp"
#include "io/PgmIO.hpp"
#include "io/PngIO.hpp"
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

std::vector<MatrixXb> read_video_dir(const std::vector<std::string>& filenames);

std::vector<Vector2i> bresenham(const Vector2i& p1, const Vector2i& p2);

int main(int argc, char** argv)
{
    parse_commandline(argv);

    // Read in PNG's, convert to grayscale
    auto filenames = generate_filenames(format, img_start, img_count);
    auto grays = read_video_dir(filenames);

    // Compute average img
    auto avg = average(grays);

    // Compute sagittal view
    auto points = bresenham({start_x, start_y}, {end_x, end_y});
    MatrixXb slice_img(grays.size() * 2, points.size());
    for (size_t i = 0; i < grays.size(); ++i) {

        // Subtract background
        auto diff = math::abs(grays[i] - avg).clip(0, 255).as_type<uint8_t>();

        // Threshold away low 10%
        auto thresh = threshold_binary(diff, uint8_t(0.1 * 255));
        for (size_t p = 0; p < points.size(); ++p) {
            slice_img(2 * i, p) = thresh(points[p][1], points[p][0]);
            slice_img(2 * i + 1, p) = thresh(points[p][1], points[p][0]);
        }
    }

    // Median blur
    auto median_ksize = int32_t(0.02 * points.size());
    if (median_ksize % 2 == 0) {
        median_ksize += 1;
    }
    std::cout << "median filter size: " << median_ksize << std::endl;
    slice_img = median_filter(slice_img, median_ksize, median_ksize);

    // Dilate by square
    auto morph_ksize = int32_t(0.04 * points.size());
    slice_img = morphology::dilate(
        slice_img, morphology::kernels::rectangle(morph_ksize, morph_ksize));

    PgmIO::write(slice_img, "reslice.pgm");
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

std::vector<MatrixXb> read_video_dir(const std::vector<std::string>& filenames)
{
    std::vector<MatrixXb> gray_pngs;
    gray_pngs.reserve(filenames.size());
    for (const auto& f : filenames) {
        auto color = PngIO::read(f);
        gray_pngs.push_back(color_to_grayscale(color));
    }

    return gray_pngs;
}

// Implementation taken from:
// https://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/FOURTH_EDITION/PROGRAMS/bresenham.c
std::vector<Vector2i> bresenham(const Vector2i& p1, const Vector2i& p2)
{
    std::vector<Vector2i> points;
    int32_t dx, dy, i, e;
    int32_t incx, incy, inc1, inc2;
    int32_t x, y;

    dx = p2[0] - p1[0];
    dy = p2[1] - p1[1];

    if (dx < 0) {
        dx = -dx;
    }
    if (dy < 0) {
        dy = -dy;
    }
    incx = 1;
    if (p2[0] < p1[0]) {
        incx = -1;
    }
    incy = 1;
    if (p2[1] < p1[1]) {
        incy = -1;
    }
    x = p1[0];
    y = p1[1];

    if (dx > dy) {
        points.push_back({x, y});
        e = 2 * dy - dx;
        inc1 = 2 * (dy - dx);
        inc2 = 2 * dy;
        for (i = 0; i < dx; i++) {
            if (e >= 0) {
                y += incy;
                e += inc1;
            } else {
                e += inc2;
            }
            x += incx;
            points.push_back({x, y});
        }
    } else {
        points.push_back({x, y});
        e = 2 * dx - dy;
        inc1 = 2 * (dx - dy);
        inc2 = 2 * dx;
        for (i = 0; i < dy; i++) {
            if (e >= 0) {
                x += incx;
                e += inc1;
            } else {
                e += inc2;
            }
            y += incy;
            points.push_back({x, y});
        }
    }

    return points;
}
