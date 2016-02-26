#include <iostream>
#include <cstring>
#include <fstream>
#include <cmath>
#include "io/BmpIO.hpp"
#include "matrix/Matrix.hpp"
#include "improc/Improc.hpp"

using namespace sipl;

void parse_commandline(int argc, char** argv);
MatrixXd parse_filter(const std::string& filter_file);

// Actions available
enum class ActionType { CONVOLVE, KTH_FILTER, UNKNOWN };

// Globals for paths & action type
std::string g_infile;
std::string g_outfile;
std::string g_filter_file;
int32_t g_order = -1;
ActionType g_action = ActionType::UNKNOWN;

int main(int argc, char** argv)
{
    if (argc < 5) {
        std::cout << "Usage:" << std::endl;
        std::cout
            << "    " << argv[0]
            << " -i inputFileName -o outputFileName -f filterFileName [-m k]"
            << std::endl;
        std::exit(1);
    }

    parse_commandline(argc, argv);

    // Read input files
    const auto img = BmpIO::read(g_infile);
    const auto kernel = parse_filter(g_filter_file);

    // Perform relevant action
    switch (g_action) {
    case ActionType::CONVOLVE: {
        const auto result = convolve(img, kernel);
        BmpIO::write(result, g_outfile);
        break;
    }
    case ActionType::KTH_FILTER: {
        const int32_t nbor_height = kernel.dims[0];
        const int32_t nbor_width = kernel.dims[1];
        const int32_t k =
            (g_order == -1 ? nbor_width * nbor_height / 2 : g_order);
        const auto result =
            nonlinear_kth_filter(img, nbor_height, nbor_width, k);
        BmpIO::write(result, g_outfile);
        break;
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
        } else if (std::strncmp(argv[i], "-f", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::CONVOLVE;
            g_filter_file = std::string(argv[i]);
        } else if (std::strncmp(argv[i], "-m", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::KTH_FILTER;
            g_order = (argv[i] == nullptr ? -1 : std::stoi(argv[i]));
        }
        ++i;
    }
}

MatrixXd parse_filter(const std::string& filter_file)
{
    std::ifstream stream(filter_file);
    if (!stream) {
        std::cerr << "Couldn't open transform file: " << filter_file
                  << std::endl;
        std::exit(1);
    }

    // Read size data first
    int32_t kwidth, kheight;
    stream >> kwidth;
    stream >> kheight;

    // Read in values one element at a time
    MatrixXd m(kheight, kwidth);
    double sum = 0;
    for (int32_t i = 0; i < kheight; ++i) {
        for (int32_t j = 0; j < kwidth; ++j) {
            stream >> m(i, j);
            sum += m(i, j);
        }
    }

    // Normalize if necessary
    constexpr double eps = 1e-10;
    if (std::fabs(sum) >= eps) {
        m /= double(m.abssum());
    }

    return m;
}
