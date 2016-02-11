#include "io/PGMReader.hpp"

using namespace sipl;

PGMReader::PGMReader(const std::string& filename)
    : filename_(filename), image_width_(0), image_height_(0), maxval_(0)
{
    type_ = determine_file_type();
}

// Figure out whether this is binary or ascii. Need to open file once to
// look at magic number to determine file type
PGMReader::PType PGMReader::determine_file_type() const
{
    std::ifstream stream{filename_};
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
void PGMReader::process_header(std::ifstream& stream)
{
    // Check for comments, etc
    std::string word, comment_line;
    stream >> word;
    while ('#' == word[0]) {
        std::getline(stream, comment_line);
        stream >> word;
    }

    image_width_ = size_t(std::stoi(word));

    stream >> word;
    while ('#' == word[0]) {
        std::getline(stream, comment_line);
        stream >> word;
    }

    image_height_ = size_t(std::stoi(word));

    stream >> word;
    while ('#' == word[0]) {
        std::getline(stream, comment_line);
        stream >> word;
    }

    maxval_ = std::stoi(word);
}
