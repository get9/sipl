#include <iostream>
#include <cstring>
#include "io/ImageIO.hpp"
#include "io/PpmIO.hpp"
#include "io/PgmIO.hpp"
#include "improc/Improc.hpp"

using namespace sipl;

Matrix33d parse_transform(const std::string& filename);
void parse_commandline(int32_t, char** argv);

// Possible actions
enum class ActionType { COLOR_CONVERT, TRANSFORM, UNKNOWN };

// Globals for command line args
std::string g_infile;
std::string g_outfile;
std::string g_transform_file;
ActionType g_action = ActionType::UNKNOWN;
auto g_interpolate_type = InterpolateType::UNKNOWN;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0] << " -i inputFileName -o outputFileName "
                                          "[-c|{-p transformFileName N|B}]"
                  << std::endl;
        std::exit(1);
    }

    // Parse command line args
    parse_commandline(argc, argv);
    FileType ftype = ImageIO::file_type(g_infile);

    // Do work
    switch (g_action) {
    // Convert from rgb --> gray
    case ActionType::COLOR_CONVERT:
        if (ftype == FileType::PGM) {
            std::cerr << "[error]: trying to convert grayscale image"
                      << std::endl;
            std::exit(1);
        } else {
            auto gray_mat = color_to_grayscale(PpmIO::read(g_infile));
            PgmIO::write(gray_mat, g_outfile);
        }
        break;

    // Perform projective transform
    case ActionType::TRANSFORM: {
        auto transform = parse_transform(g_transform_file);

        switch (g_interpolate_type) {
        // Bilinear inteprolation
        case InterpolateType::BILINEAR: {
            if (ftype == FileType::PGM) {
                auto img = PgmIO::read(g_infile);
                auto new_mat =
                    projective_transform<BilinearInterpolator<double>>(
                        img, transform);
                PgmIO::write(new_mat, g_outfile);
            } else {
                auto img = PpmIO::read(g_infile);
                auto new_mat =
                    projective_transform<BilinearInterpolator<Vector3d>>(
                        img, transform);
                PpmIO::write(new_mat, g_outfile);
            }
            break;
        }

        // Nearest neighbor inteprolation
        case InterpolateType::NEAREST_NEIGHBOR: {
            if (ftype == FileType::PGM) {
                auto img = PgmIO::read(g_infile);
                auto new_mat =
                    projective_transform<NearestNeighborInterpolator<double>>(
                        img, transform);
                PgmIO::write(new_mat, g_outfile);
            } else {
                auto img = PpmIO::read(g_infile);
                auto new_mat =
                    projective_transform<NearestNeighborInterpolator<Vector3d>>(
                        img, transform);
                PpmIO::write(new_mat, g_outfile);
            }
            break;
        }

        case InterpolateType::UNKNOWN:
            std::cerr << "[error]: Unknown interpolate type. Use one of [N, B]"
                      << std::endl;
            std::exit(1);
        }
        break;
    }

    // Error
    case ActionType::UNKNOWN:
        std::cerr << "[error]: unknown action type" << std::endl;
        std::exit(1);
    }
}

Matrix33d parse_transform(const std::string& filename)
{
    std::ifstream stream{filename};
    if (!stream) {
        std::cerr << "Couldn't open transform file: " << filename << std::endl;
        std::exit(1);
    }

    Matrix33d m;
    std::string line;
    for (int32_t i = 0; i < 3; ++i) {
        std::getline(stream, line);
        std::stringstream ss{line};
        ss >> m(i, 0);
        ss >> m(i, 1);
        ss >> m(i, 2);
    }
    return m;
}

void parse_commandline(int32_t argc, char** argv)
{
    int32_t i = 1;
    while (argv[i] != nullptr && i < argc) {
        if (std::strncmp(argv[i], "-i", std::strlen(argv[i])) == 0) {
            ++i;
            g_infile = std::string(argv[i]);
        } else if (std::strncmp(argv[i], "-o", std::strlen(argv[i])) == 0) {
            ++i;
            g_outfile = std::string(argv[i]);
        } else if (std::strncmp(argv[i], "-c", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::COLOR_CONVERT;
        } else if (std::strncmp(argv[i], "-p", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::TRANSFORM;
            g_transform_file = std::string(argv[i]);
            ++i;
            if (std::strncmp(argv[i], "N", 1) == 0) {
                g_interpolate_type = InterpolateType::NEAREST_NEIGHBOR;
            } else if (std::strncmp(argv[i], "B", 1) == 0) {
                g_interpolate_type = InterpolateType::BILINEAR;
            } else {
                g_interpolate_type = InterpolateType::UNKNOWN;
            }
        }
        ++i;
    }
}
