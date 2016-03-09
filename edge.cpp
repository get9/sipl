#include <iostream>
#include <cmath>
#include "io/BmpIO.hpp"
#include "matrix/Matrix"
#include "improc/Improc"

using namespace sipl;

void parse_commandline(int argc, char** argv);

// Actions available
enum class ActionType { SOBEL, PREWITT, CANNY, UNKNOWN };

// Globals for paths & action type
std::string g_infile;
std::string g_outfile;
int32_t g_threshold = 0;
double g_sigma = 0;
double g_t0 = 0;
double g_t1 = 0;
double g_t2 = 0;
ActionType g_action = ActionType::UNKNOWN;

int main(int argc, char** argv)
{
    if (argc < 5) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0]
                  << " -i inputFileName -o outputFileName {-s t} | {-p t} | "
                     "{-c sigma t0 t1 t2}"
                  << std::endl;
        std::exit(1);
    }

    parse_commandline(argc, argv);

    // Read input files
    const auto img = BmpIO::read(g_infile);

    // Perform relevant action
    switch (g_action) {
    case ActionType::SOBEL: {
        auto abs_f = [](auto e) { return std::abs(e); };
        auto grad_x = convolve<double>(img, Kernels::SobelX).apply(abs_f);
        auto grad_y = convolve<double>(img, Kernels::SobelY).apply(abs_f);
        BmpIO::write(grad_x.as_type<uint8_t>(), "gradx.bmp");
        BmpIO::write(grad_y.as_type<uint8_t>(), "grady.bmp");
        auto res = grad_x + grad_y;
        BmpIO::write(res.as_type<uint8_t>(), g_outfile);
        break;
    }
    case ActionType::PREWITT: {
        // BmpIO::write(threshold(result, g_threshold), g_outfile);
        std::cerr << "[error]: not implemented yet" << std::endl;
        std::exit(1);
        break;
    }
    case ActionType::CANNY: {
        std::cerr << "[error]: not implemented yet" << std::endl;
        std::exit(1);
    }
    case ActionType::UNKNOWN:
        std::cerr << "[error]: unknown action type" << std::endl;
        std::exit(1);
    }
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
        } else if (std::strncmp(argv[i], "-s", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::SOBEL;
            g_threshold = std::stod(argv[i]);
        } else if (std::strncmp(argv[i], "-p", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::PREWITT;
            g_threshold = std::stod(argv[i]);
        } else if (std::strncmp(argv[i], "-p", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::CANNY;
            g_sigma = std::stod(argv[++i]);
            g_t0 = std::stod(argv[++i]);
            g_t1 = std::stod(argv[++i]);
            g_t2 = std::stod(argv[++i]);
        } else {
            g_action = ActionType::UNKNOWN;
        }
        ++i;
    }
}
