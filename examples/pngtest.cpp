#include "io/PngIO.hpp"
#include "io/PpmIO.hpp"
#include "matrix/Statistics.hpp"
#include "improc/Filter.hpp"
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

int main(int argc, char** argv)
{
    parse_commandline(argv);

    // Read in PNG's, convert to grayscale
    auto filenames = generate_filenames(format, img_start, img_count);
    auto grays = read_video_dir(filenames);

    // Compute median img
    auto avg = median(grays);
    std::cout << "done computing median" << std::endl;
    auto diff = math::abs(grays[0] - avg).clip(0, 255).as_type<RgbPixel>();

    PngIO::write(diff, "median.png");
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
        std::cout << "read: " << f << std::endl;
        gray_pngs.push_back(color_to_grayscale(color));
    }
    std::cout << "Done with reading" << std::endl;

    return gray_pngs;
}
