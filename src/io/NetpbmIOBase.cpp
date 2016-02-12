#include <sstream>
#include "io/NetpbmIOBase.hpp"

using namespace sipl;

// Process the header of both ASCII and Binary files
std::tuple<size_t, size_t, size_t> NetpbmIOBase::process_header(
    std::ifstream& stream)
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
