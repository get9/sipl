#include <iostream>
#include <tuple>
#include "io/PGMIO.hpp"

using namespace sipl;

// Figure out whether this is binary or ascii. Need to open file once to
// look at magic number to determine file type
PGMIO::PType PGMIO::determine_file_type(const std::string& filename) const
{
    std::ifstream stream{filename};
    if (!stream.is_open()) {
        throw IOException{"Could not open file to determine file type"};
    }

    std::string type;
    stream >> type;
    if ("P5" == type) {
        return PType::BINARY;
    } else if ("P2" == type) {
        return PType::ASCII;
    } else {
        return PType::UNKNOWN;
    }
}

// Process the header of both ASCII and Binary files
std::tuple<size_t, size_t, size_t> PGMIO::process_header(
    std::ifstream& stream) const
{
    // Get magic number (must be first string in header)
    std::string _;
    stream >> _;

    // Check for comments, etc
    std::string word, comment_line;
    stream >> word;
    while ('#' == word[0]) {
        std::getline(stream, comment_line);
        stream >> word;
    }

    size_t image_width = size_t(std::stoul(word));

    stream >> word;
    while ('#' == word[0]) {
        std::getline(stream, comment_line);
        stream >> word;
    }

    size_t image_height = size_t(std::stoul(word));

    stream >> word;
    while ('#' == word[0]) {
        std::getline(stream, comment_line);
        stream >> word;
    }

    size_t maxval = std::stoul(word);

    return std::make_tuple(image_height, image_width, maxval);
}
