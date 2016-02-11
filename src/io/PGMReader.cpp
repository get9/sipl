#include "PGMReader.hpp"

using namespace sipl;

PGMReader::PGMReader(const std::string& filename)
    : filename_(filename), image_width_(0), image_height_(0), maxval_(0)
{
    type_ = determine_file_type();
    process_header();
}

// Figure out whether this is binary or ascii. Need to open file once to
// look at magic number to determine file type
PGMReader::PType PGMReader::determine_file_type()
{
    std::ifstream stream{filename_};
    if (!stream.is_open()) {
        throw IOException { "Could not open file to determine file type" }
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
void PGMReader::process_header(void)
{
    std::ifstream stream{filename_};
    if (!stream.is_open()) {
        throw IOException{"Could not open file for reading header"};
    }

    // Check for comments, etc
    std::string word, comment_line;
    stream >> word;
    while ("#" == word.begin()) {
        std::getline(stream, comment_line);
        stream >> word;
    }

    width_ = size_t(std::stoi(word));

    stream >> word;
    while ("#" == word.begin()) {
        std::getline(stream, comment_line);
        stream >> word;
    }

    height_ = size_t(std::stoi(word));

    stream >> word;
    while ("#" == word.begin()) {
        std::getline(stream, comment_line);
        stream >> word;
    }

    maxval_ = std::stoi(word);
}
