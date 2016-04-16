#include "Util.hpp"
#include "improc/Filter.hpp"
#include "improc/Label.hpp"
#include "improc/Morphology.hpp"
#include "io/PgmIO.hpp"
#include "io/PngIO.hpp"
#include "matrix/Statistics.hpp"
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int32_t start_x, start_y, end_x, end_y;
int32_t img_start, img_count;
std::string format;

void parse_commandline(char** argv);

std::vector<std::string> generate_filenames(const std::string& format,
                                            int32_t start,
                                            int32_t count);

std::vector<sipl::MatrixXb> read_video_dir(
    const std::vector<std::string>& filenames);

std::vector<sipl::Vector2i> bresenham(const sipl::Vector2i& p1,
                                      const sipl::Vector2i& p2);

double average_mass(const std::vector<sipl::Component>& cs);

constexpr uint8_t THRESH_VAL = uint8_t(0.0875 * 255);
constexpr double MORPH_KERNEL_SIZE_PERC = 0.03;
constexpr double REMOVE_BLOB_MASS_PERC = 0.2;
constexpr double SPLIT_BLOB_FACTOR = 1.5;

int main(int argc, char** argv)
{
    if (argc < 8) {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "    " << argv[0]
                  << " LOI_x0 LOI_y0 LOI_x1 LOI_y1 image_name_template "
                     "start_number total_image_number"
                  << std::endl;
        std::exit(1);
    }

    parse_commandline(argv);

    // Read in PNG's, convert to grayscale
    auto filenames = generate_filenames(format, img_start, img_count);
    auto grays = read_video_dir(filenames);

    // Compute background img
    auto bg = sipl::average(grays);

    // Compute sagittal view
    auto points = bresenham({start_x, start_y}, {end_x, end_y});
    sipl::MatrixXb slice_img(grays.size() * 2, points.size());
    for (size_t i = 0; i < grays.size(); ++i) {

        // Subtract background
        auto diff =
            sipl::math::abs(grays[i] - bg).clip(0, 255).as_type<uint8_t>();

        // Threshold away low 10%
        auto thresh_img = sipl::threshold_binary(diff, THRESH_VAL);
        for (size_t p = 0; p < points.size(); ++p) {
            slice_img(2 * i, p) = thresh_img(points[p][1], points[p][0]);
            slice_img(2 * i + 1, p) = thresh_img(points[p][1], points[p][0]);
        }
    }
    grays.reserve(0);
    grays.shrink_to_fit();

    // Median blur
    auto median_ksize = int32_t(0.02 * points.size());
    if (median_ksize % 2 == 0) {
        median_ksize += 1;
    }
    slice_img = sipl::median_filter(slice_img, median_ksize, median_ksize);

    // Dilate by square
    auto morph_ksize = int32_t(MORPH_KERNEL_SIZE_PERC * points.size());
    slice_img = sipl::morphology::dilate(
        slice_img,
        sipl::morphology::kernels::rectangle(morph_ksize, morph_ksize));

    // Find connected components
    auto components =
        sipl::connected_components(slice_img, sipl::Connectivity::N8);

    // Filter any blobs < REMOVE_BLOB_MASS_PERC% of the avg mass
    double avg_mass = sipl::average_mass(components);
    auto it = std::partition(
        std::begin(components), std::end(components), [avg_mass](auto c) {
            return c.mass >= REMOVE_BLOB_MASS_PERC * avg_mass;
        });
    std::vector<sipl::Component> large_blobs(std::begin(components), it);
    std::vector<sipl::Component> small_blobs(it, std::end(components));

    // Draw over small blobs and remove them from image
    for (const auto& blob : small_blobs) {
        for (const auto& i : blob.indices) {
            slice_img(i[0], i[1]) = 0;
        }
    }

    // Recalculate avg_mass
    avg_mass = sipl::average_mass(large_blobs);
    for (size_t i = 0; i < large_blobs.size(); ++i) {
        std::cout << i + 1 << "\tmass: " << large_blobs[i].mass
                  << "\tcom: " << large_blobs[i].center_of_mass << std::endl;
    }

    // Get a new count
    size_t car_count = large_blobs.size();
    for (const auto& c : large_blobs) {
        if (c.mass > SPLIT_BLOB_FACTOR * avg_mass) {
            car_count++;
        }
    }

    // Report
    std::cout << "car count: " << car_count << std::endl;
    std::cout << "Crossings:" << std::endl;
    for (const auto& c : large_blobs) {
        int32_t cross_count = (c.mass > SPLIT_BLOB_FACTOR * avg_mass ? 2 : 1);
        std::cout << cross_count << " cross at time "
                  << int32_t(std::round(c.center_of_mass[0] / 2)) << std::endl;
    }
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

std::vector<sipl::MatrixXb> read_video_dir(
    const std::vector<std::string>& filenames)
{
    std::vector<sipl::MatrixXb> gray_pngs;
    gray_pngs.reserve(filenames.size());
    for (const auto& f : filenames) {
        auto color = sipl::PngIO::read(f);
        gray_pngs.push_back(color_to_grayscale(color));
    }

    return gray_pngs;
}

// Implementation taken from:
// https://www.cs.unm.edu/~angel/BOOK/INTERACTIVE_COMPUTER_GRAPHICS/FOURTH_EDITION/PROGRAMS/bresenham.c
std::vector<sipl::Vector2i> bresenham(const sipl::Vector2i& p1,
                                      const sipl::Vector2i& p2)
{
    std::vector<sipl::Vector2i> points;
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
