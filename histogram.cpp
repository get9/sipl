#include <iostream>
#include "io/PgmIO.hpp"
#include "improc/Histogram.hpp"

using namespace sipl;

void parse_commandline(int argc, char** argv);

// Actions available
enum class ActionType { EQUALIZE, PLOT, MATCH, UNKNOWN };

// Globals for paths & action type
std::string g_infile;
std::string g_outfile;
std::string g_matchfile;
ActionType g_action = ActionType::UNKNOWN;

int main(int argc, char** argv)
{
    if (argc < 5) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0] << " -i inputFileName -o outputFileName "
                                          "[-e|-p|-m imageB]"
                  << std::endl;
        std::exit(1);
    }

    parse_commandline(argc, argv);

    const auto img = PgmIO::read(g_infile);

    switch (g_action) {
    case ActionType::EQUALIZE: {
        const auto equalized_img = equalize_hist(img);
        PgmIO::write(equalized_img, g_outfile);
        break;
    }
    case ActionType::PLOT: {
        const auto hist = histogram(img);
        const auto hist_img = hist_to_img(hist);
        PgmIO::write(hist_img, g_outfile);
        break;
    }
    case ActionType::MATCH: {
        const auto target_img = PgmIO::read(g_matchfile);
        const auto matched_img = histogram_match(target_img, img);
        PgmIO::write(matched_img, g_outfile);
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
        } else if (std::strncmp(argv[i], "-e", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::EQUALIZE;
        } else if (std::strncmp(argv[i], "-p", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::PLOT;
        } else if (std::strncmp(argv[i], "-m", std::strlen(argv[i])) == 0) {
            ++i;
            g_action = ActionType::MATCH;
            ++i;
            g_matchfile = std::string(argv[i]);
        }
        ++i;
    }
}
